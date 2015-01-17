#ifndef __arag__CommandEvents__
#define __arag__CommandEvents__

#include <list>
#include <string>

namespace arag
{

class ISubscriber;
    
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
    
    void subscribe(ISubscriber* subscr);
    
    void unsubscribe(ISubscriber* subscr);
    
    void fire(Event event, const std::string& key, int db);
    
private:
    std::list<ISubscriber*> mSubscribers;
};

class ISubscriber
{
public:
    
    virtual void notify(EventPublisher::Event event, const std::string& key, int db) = 0;
};
};

#endif /* defined(__arag__CommandEvents__) */
