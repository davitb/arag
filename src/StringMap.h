#ifndef __arag__StringMap__
#define __arag__StringMap__

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "SelfTest.h"

namespace arag
{

/*
    Implements all low level data structures and commands.
    Refer to http://redis.io/commands for more information.
 */
class StringMap
{
public:
    
    enum
    {
        KEYS_AND_VALUES,
        KEYS,
        VALUES
    };
    
    enum ExpirationType
    {
        SEC,
        MSEC
    };

    enum SetKeyPolicy
    {
        CREATE_IF_DOESNT_EXIST,
        ONLY_IF_ALREADY_EXISTS,
        ONLY_IF_DOESNT_ALREADY_EXISTS
    };
    
    StringMap();
    
    int set(std::string key,
            std::string value,
            ExpirationType expType = SEC,
            int exp = 0,
            SetKeyPolicy policy = CREATE_IF_DOESNT_EXIST);
    
    std::string get(std::string key);

    bool deleteKey(std::string key);
    
    std::string getset(std::string key, std::string value);
    
    int append(std::string key, std::string value);
    
    std::string getRange(std::string key, int start, int end);
    
    int incrBy(std::string key, int by);

    std::string incrBy(std::string key, double by);
    
    std::vector<std::pair<std::string, int>> mget(const std::vector<std::string>& keys);
    
    std::vector<std::pair<std::string, int>> getAll(int getAllType);
    
    int len();
    
    int getCounter() const;
    
    void cleanup();
    
private:
    
    class Item
    {
    public:
        std::string strVal;
        int timestamp;
        int exp;
        ExpirationType expType;

        Item();
        Item(std::string val, ExpirationType expType, int exp);
    };
    
    friend class SelfTest;
    
    std::unordered_map<std::string, Item> map;
    std::recursive_mutex mLock;
    int counter;
};
    
}; // __arag__StringMap__


#endif /* defined(__CacheServer__Map__) */
