#ifndef __CacheServer__RequestProcessor__
#define __CacheServer__RequestProcessor__

#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include "Commands.h"

using namespace std;
using namespace cache_server;

namespace cache_server
{

class RequestProcessor
{
public:

    enum
    {
        THREAD_COUNT = 2,
        TRIGGER_CLEANUP_LIMIT = 20
    };
    
    enum RequestType
    {
        INTERNAL,
        EXTERNAL
    };
    
    enum ResultType
    {
        SKIP,
        STOP,
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
    
    class ProcessingUnit
    {
    public:
        std::thread thd;
        std::mutex lock;
        std::queue<Request> que;
        std::condition_variable cond;
    };
    
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
    std::vector<ProcessingUnit> mPunits;
    CSMap mData;
    int mThreadCount;
    int mTriggerCleanupLimit;
    
private:
    
    friend class SelfTest;
    
};
    
};

#endif /* defined(__CacheServer__RequestProcessor__) */
