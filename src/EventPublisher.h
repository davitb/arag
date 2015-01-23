#ifndef __arag__CommandEvents__
#define __arag__CommandEvents__

#include <unordered_map>
#include <string>

namespace arag
{

class ISubscriber;
    
/*
    Upon changing database state arag commands fire appropriate events.
    This class defined the events and also is used to fire them.
*/
class EventPublisher
{
public:
    
    enum Event
    {
        del,
        rename_from,
        rename_to,
        expire,
        sortstore,
        set,
        setrange,
        incrby,
        incrbyfloat,
        append,
        lpush,
        rpush,
        rpop,
        lpop,
        linsert,
        lset,
        ltrim,
        hset,
        hincrby,
        hincrbyfloat,
        hdel,
        sadd,
        srem,
        spop,
        sinterstore,
        sunionostore,
        sdiffstore,
        zincr,
        zadd,
        zrem,
        zrembyscore,
        zrembyrank,
        zinterstore,
        zunionstore,
        expired,
        evicted
    };
    
    // Add the given subscriber to the map
    void subscribe(ISubscriber* subscr);

    // Remove the given subscriber
    void unsubscribe(ISubscriber* subscr);
    
    // Fire given event, key and db to all subscribers
    void fire(Event event, const std::string& key, int db);
    
private:
    std::unordered_map<ISubscriber*, bool> mSubscribers;
};

class ISubscriber
{
public:
    
    virtual void notify(EventPublisher::Event event, const std::string& key, int db) = 0;
};
};

#endif /* defined(__arag__CommandEvents__) */
