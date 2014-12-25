#ifndef __CacheServer__Map__
#define __CacheServer__Map__

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "SelfTest.h"

namespace cache_server
{
    
class CSMap
{
public:
    
    CSMap();
    
    int set(std::string key, std::string value, int expSecs = 0);
    
    std::string get(std::string key);
    
    std::string getset(std::string key, std::string value, int expSecs = 0);
    
    int append(std::string key, std::string value, int expSecs = 0);
    
    std::string getRange(std::string key, int start, int end);
    
    int incr(std::string key);
    
    std::vector<std::string> mget(std::vector<std::string> keys);
    
    int getCounter() const;
    
    void clearCounter();
    
    void cleanup();
    
private:
    
    bool isExpired(int timestamp, int expSecs);
    
private:
    
    class Item
    {
    public:
        std::string strVal;
        int intVal;
        int timestamp;
        int expSecs;

        Item();
        Item(std::string val, int exp);
    };
    
    friend class SelfTest;
    
    std::unordered_map<std::string, Item> map;
    std::recursive_mutex mLock;
    int counter;
};
    
}; // map_server


#endif /* defined(__CacheServer__Map__) */
