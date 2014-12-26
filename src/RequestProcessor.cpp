#include <iostream>
#include <random>
#include <iostream>
#include "Commands.h"
#include "RequestProcessor.h"
#include "RedisProtocol.h"

using namespace std;
using namespace cache_server;

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
        RequestProcessor::Request req(CMD_INTERNAL_STOP, RequestType::INTERNAL);
        enqueueRequest(mPunits[i], req);
        mPunits[i].thd.join();
    }
}

void RequestProcessor::enqueueRequest(Request req)
{
    // Randomly choose a processing unit
    std::random_device rd;
    std::default_random_engine e1(rd());
    std::uniform_int_distribution<int> uniform_dist(0, mThreadCount - 1);
    int nextUnit = uniform_dist(e1);
    
    cout << "Thread #: " << nextUnit << endl;
    
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
        
        punit.cond.wait(lock, [&punit] { return !punit.que.empty(); });
        
        Request req = punit.que.front();
        
        cout << "New request received: " << req.cmdLine << endl;
        
        punit.que.pop();
        
        lock.unlock();
        
        try {
            shared_ptr<Command> cmd(Command::createCommand(req.cmdLine));
            
            if (req.type == RequestType::INTERNAL) {
                ResultType rt = processInternalCommand(cmd->getCommandName());
                if (rt == ResultType::STOP) {
                    break;
                }
                if (rt == ResultType::SKIP) {
                    continue;
                }
            }
            
            string res = cmd->execute(mData);
            
            if (req.cb != nullptr) {
                req.cb(res);
            }
        }
        catch (exception& e) {
            cout << "exception occured: " << e.what() << endl;
            
            if (req.cb != nullptr) {
                req.cb(cache_server::ERR_GENERIC);
            }
        }
        
        if (mData.getCounter() > mTriggerCleanupLimit) {
            enqueueCleanup();
        }
    }
}

RequestProcessor::ResultType RequestProcessor::processInternalCommand(std::string cmd)
{
    if (cmd == CMD_INTERNAL_STOP || cmd == CMD_EXTERNAL_EXIT) {
        cout << "Stopping the thread" << endl;
        return ResultType::STOP;
    }
    else
        if (cmd == CMD_INTERNAL_CLEANUP) {
            cout << "Trigerring cleanup" << endl;
            try {
                mData.cleanup();
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
    Request req(CMD_INTERNAL_CLEANUP, RequestType::INTERNAL);
    enqueueRequest(req);
}

