#ifndef __CacheServer__RequestProcessor__
#define __CacheServer__RequestProcessor__

#include <string>
#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include "Commands.h"

using namespace std;
using namespace arag;

namespace arag
{

/*
    This class is responsible for processing Redis requests.
    It maintains a thread pool and tries to distribute the load amongs them.
 */
class RequestProcessor
{
public:

    enum
    {
        // Default number of threads
        THREAD_COUNT = 1,
        // The number of added elements after which cleanup will be triggered
        TRIGGER_CLEANUP_LIMIT = 2000
    };
    
    class Request
    {
    public:
        
        shared_ptr<Command> mCommand;
        int mSessionID;

        Request(shared_ptr<Command> cmd, int sessionID);
    };
    
private:
    
    /*
        This is the state maintained by each thread.
     */
    class ProcessingUnit
    {
    public:
        std::thread thd;
        std::mutex lock;
        std::list<Request> que;
        std::condition_variable cond;
    };
    
    // Thread function
    void processingThread(ProcessingUnit& punit);

    void enqueueRequest(ProcessingUnit& punit, Request req);
    
public:
    
    RequestProcessor(int threadCount = THREAD_COUNT);
    
    void startThreads();
    
    void stopThreads();
    
    void enqueueRequest(Request req);
    
private:
    // Threads (processing units)
    std::vector<ProcessingUnit> mPunits;
    int mThreadCount;
    int mTriggerCleanupLimit;
    
private:
    
    friend class SelfTest;
    
};
    
};

#endif /* defined(__CacheServer__RequestProcessor__) */
