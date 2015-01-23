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

RequestProcessor::Request::Request(shared_ptr<Command> cmd,
                                   int sessionID)
                                    : mCommand(cmd), mSessionID(sessionID)
{
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
        stopCmd->setType(Command::Type::INTERNAL);
        
        RequestProcessor::Request req(stopCmd, SessionContext::Consts::FAKE_SESSION);
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

void RequestProcessor::processingThread(ProcessingUnit& punit)
{
    while (true) {
        unique_lock<mutex> lock(punit.lock);
        
        // Wait for the conditional variable
        punit.cond.wait(lock, [&punit] { return !punit.que.empty(); });
        
        Request req = punit.que.front();
        punit.que.pop_front();
        
        lock.unlock();

        try {
            Command::executeEndToEnd(req.mCommand, req.mSessionID);
        }
        catch (std::exception& e) {
            cout << e.what() << endl;
            break;
        }
    }
}
