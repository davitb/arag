#include <iostream>
#include <iostream>
#include "Commands.h"
#include "StringMap.h"
#include "Utils.h"
#include "RequestProcessor.h"
#include "RedisProtocol.h"
#include "AragServer.h"

using namespace std;
using namespace arag;

//-----------------------------------------------------------------

RequestProcessor::Request::Request(Command& cmd,
                                   RequestType type,
                                   int sessionID)
                                    : mCommand(cmd)
{
    mType = type;
    mSessionID = sessionID;
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
        shared_ptr<Command> stopCmd = std::make_shared<InternalCommand>(command_const::CMD_INTERNAL_STOP);
        RequestProcessor::Request req(*stopCmd.get(),
                                      RequestType::INTERNAL,
                                      SessionContext::Consts::FAKE_SESSION);
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
    punit.que.push_back(req);
    punit.cond.notify_one();
}

RequestProcessor::Request RequestProcessor::extractNextRequest(std::list<Request>& que)
{
    Request req = que.front();
    que.pop_front();
    return req;
}

void RequestProcessor::processingThread(ProcessingUnit& punit)
{
    while (true) {
        unique_lock<mutex> lock(punit.lock);
        
        // Wait for the conditional variable
        punit.cond.wait(lock, [&punit] { return !punit.que.empty(); });
        
        Request req = extractNextRequest(punit.que);
        ClientSession& session = Arag::instance().getClientSession(req.mSessionID);
        SessionContext sessionCtx = session.getContext();
        int selectedDBIndex = sessionCtx.getDatabaseIndex();
        InMemoryData& selectedDB = Database::instance().get(selectedDBIndex);
        
        lock.unlock();
        
        try {
            // Create appropriate command
            Command& cmd = req.mCommand;
            
            // Check if this is an internal operation and execute it
            if (req.mType == RequestType::INTERNAL) {
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
            
            string res = cmd.execute(selectedDB, sessionCtx);
            
            if (res.length() != 0 && req.mType != RequestType::INTERNAL) {
                session.writeResponse(res);
            }
        }
        catch (exception& e) {
            //cout << "exception occured: " << e.what() << endl;
            //cout << "command: " << cmdLine << endl;
            
            if (req.mType != RequestType::INTERNAL) {
                session.writeResponse(redis_const::ERR_GENERIC);
            }
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

