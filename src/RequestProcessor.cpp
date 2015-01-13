#include <iostream>
#include <iostream>
#include "Commands.h"
#include "StringMap.h"
#include "Utils.h"
#include "RequestProcessor.h"
#include "RedisProtocol.h"

using namespace std;
using namespace arag;

//-----------------------------------------------------------------

RequestProcessor::Request::Request(const std::string& cmdLine,
                                   RequestType type,
                                   SessionContext& sessionCtx,
                                   function<void(std::string)> cb) : mSessionCtx(sessionCtx)
{
    this->cmdLine = cmdLine;
    this->type = type;
    this->cb = cb;
}

RequestProcessor::Request::Request(const std::string& cmdLine,
                                   RequestType type)  : mSessionCtx(fakeCtx)
{
    this->cmdLine = cmdLine;
    this->type = type;
    this->cb = nullptr;
}

//-----------------------------------------------------------------

RequestProcessor::RequestProcessor(int threadCount)
{
    mThreadCount = threadCount;
    mTriggerCleanupLimit = TRIGGER_CLEANUP_LIMIT;
}

void RequestProcessor::startThreads()
{
    mPunits = vector<ProcessingUnit>(mThreadCount);
    
    cout << "Starting all threads" << endl;
    
    for (int i = 0; i < mPunits.size(); ++i) {
        mPunits[i].thd = thread(&RequestProcessor::processingThread, this, std::ref(mPunits[i]));
    }
}

void RequestProcessor::stopThreads()
{
    cout << "Stopping all threads" << endl;
    
    for (int i = 0; i < mPunits.size(); ++i) {
        RequestProcessor::Request req(command_const::CMD_INTERNAL_STOP,
                                      RequestType::INTERNAL);
        enqueueRequest(mPunits[i], req);
        mPunits[i].thd.join();
    }
}

void RequestProcessor::enqueueRequest(Request req)
{
    // Randomly choose a processing unit
    int nextUnit = Utils::genRandom(0, mThreadCount - 1);
    
    //cout << "Thread #: " << nextUnit << endl;
    
    enqueueRequest(mPunits[nextUnit], req);
}

void RequestProcessor::enqueueRequest(ProcessingUnit& punit, Request req)
{
    lock_guard<mutex> lock(punit.lock);
    punit.que.push(req);
    punit.cond.notify_one();
}

void RequestProcessor::processingThread(ProcessingUnit& punit)
{
    while (true) {
        unique_lock<mutex> lock(punit.lock);
        
        // Wait for the conditional variable
        punit.cond.wait(lock, [&punit] { return !punit.que.empty(); });
        
        Request req = punit.que.front();
        int selectedDBIndex = req.mSessionCtx.get().getDatabaseIndex();
        InMemoryData& selectedDB = Database::instance().get(selectedDBIndex);
        
        //cout << "New request received: " << req.cmdLine << endl;
        
        punit.que.pop();
        
        lock.unlock();
        
        try {
            // Create appropriate command
            Command& cmd = Command::getCommand(req.cmdLine);
            
            // Check if this is an internal operation and execute it
            if (req.type == RequestType::INTERNAL) {
                ResultType rt = processInternalCommand(cmd.getCommandName());
                if (rt == ResultType::STOP) {
                    break;
                }
                if (rt == ResultType::SKIP) {
                    continue;
                }
            }
            
            // Execute the command
            
            if (!cmd.isKeyTypeValid(selectedDB)) {
                throw invalid_argument("Wrong key operation");
            }
            
            string res = cmd.execute(selectedDB, req.mSessionCtx);
            
            // Call the callback to write the response to socket
            if (req.cb != nullptr && res.length() != 0) {
                req.cb(res);
            }
        }
        catch (exception& e) {
//            cout << "exception occured: " << e.what() << endl;
//            cout << "command: " << req.cmdLine << endl;
            
            if (req.cb != nullptr) {
                req.cb(redis_const::ERR_GENERIC);
            }
        }
        
        if (selectedDB.getCounter() > mTriggerCleanupLimit) {
            //enqueueCleanup();
        }
    }
}

RequestProcessor::ResultType RequestProcessor::processInternalCommand(std::string cmd)
{
    if (cmd == command_const::CMD_INTERNAL_STOP || cmd == command_const::CMD_EXTERNAL_EXIT) {
        cout << "Stopping the thread" << endl;
        return ResultType::STOP;
    }
    else
    if (cmd == command_const::CMD_INTERNAL_CLEANUP) {
        cout << "Trigerring cleanup" << endl;
        try {
            //mData.cleanup();
        }
        catch (exception& e) {
            cout << "exception occured: " << e.what() << endl;
        }
        return ResultType::SKIP;
    }
    
    return ResultType::CONTINUE;
}

void RequestProcessor::enqueueCleanup()
{
//    Request req(command_const::CMD_INTERNAL_CLEANUP, RequestType::INTERNAL);
//    enqueueRequest(req);
}

