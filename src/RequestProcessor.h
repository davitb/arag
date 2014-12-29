#ifndef __CacheServer__RequestProcessor__
#define __CacheServer__RequestProcessor__

#include <string>
#include <thread>
#include <mutex>
#include <queue>
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
        THREAD_COUNT = 2,
        // The number of added elements after which cleanup will be triggered
        TRIGGER_CLEANUP_LIMIT = 2000
    };
    
    // There can be two types of operations.
    enum RequestType
    {
        INTERNAL,
        EXTERNAL
    };
    
    // Internal operations will return intent.
    enum ResultType
    {
        // Skip further execution
        SKIP,
        // Stop the thread
        STOP,
        // Continue with execution
        CONTINUE
    };
    
    class Request
    {
    public:
        std::string cmdLine;
        function<void(std::string)> cb;
        RequestType type;
        
        Request(std::string cmdLine, RequestType type, function<void(std::string)> cb) {
            this->cmdLine = cmdLine;
            this->type = type;
            this->cb = cb;
        }
        
        Request(std::string cmdLine, RequestType type) {
            this->cmdLine = cmdLine;
            this->type = type;
            this->cb = nullptr;
        }
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
        std::queue<Request> que;
        std::condition_variable cond;
    };
    
    // Thread function
    void processingThread(ProcessingUnit& punit);

    void enqueueRequest(ProcessingUnit& punit, Request req);
    
    void enqueueCleanup();
    
    ResultType processInternalCommand(std::string cmd);
    
public:
    
    RequestProcessor(int threadCount = THREAD_COUNT);
    
    void startThreads();
    
    void stopThreads();
    
    void enqueueRequest(Request req);
    
private:
    // Threads (processing units)
    std::vector<ProcessingUnit> mPunits;
    // Arag Map. This variable holds all the customer data
    InMemoryData mData;
    int mThreadCount;
    int mTriggerCleanupLimit;
    
private:
    
    friend class SelfTest;
    
};
    
};

#endif /* defined(__CacheServer__RequestProcessor__) */
