#ifndef __arag__Database__
#define __arag__Database__

#include "InMemoryData.h"

namespace arag
{

/*
    This singleton has two responsibilities:
    1) Keeping a list of all in-memory databases
    2) Acting as an event publisher for database state change
 */
class Database
{
public:
    
    enum
    {
        FLUSH_ALL = -1
    };
    
    static Database& instance();
    
    // Return database by db index
    InMemoryData& get(int index);
    
    // Flush the database with index. If FLUSH_ALL is specified -
    // all databases will be flushed
    void flush(int index);
    
    EventPublisher& getEventPublisher()
    {
        return mEventPublisher;
    }
    
    void initialize();

private:
    Database(int count);
    
private:
    std::vector<InMemoryData> mDatabases;
    EventPublisher mEventPublisher;
};

};

#endif /* defined(__arag__Database__) */
