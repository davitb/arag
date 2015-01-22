#ifndef __arag__Database__
#define __arag__Database__

#include "InMemoryData.h"

namespace arag
{

class Database
{
public:
    
    enum
    {
        FLUSH_ALL = -1
    };
    
    static Database& instance();
    
    Database(int count);
    
    InMemoryData& get(int index);
    
    void flush(int index);
    
    EventPublisher& getEventPublisher()
    {
        return mEventPublisher;
    }
    
private:
    std::vector<InMemoryData> mDatabases;
    EventPublisher mEventPublisher;
};

};

#endif /* defined(__arag__Database__) */
