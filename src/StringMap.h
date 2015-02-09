#ifndef __arag__StringMap__
#define __arag__StringMap__

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "IMapCommon.h"
#include "SelfTest.h"

namespace arag
{

/*
    Implements a map for storing strings and performing operations on it.
    Refer to http://redis.io/commands for more information.
 */
class StringMap : public IMapCommon
{
public:
    
    enum
    {
        KEYS_AND_VALUES,
        KEYS,
        VALUES
    };
    
    enum SetKeyPolicy
    {
        CREATE_IF_DOESNT_EXIST,
        ONLY_IFEXISTS,
        ONLY_IF_DOESNTEXISTS
    };
    
    StringMap();
    
    // Sets a key with a value
    int set(const std::string& key,
            const std::string& value,
            SetKeyPolicy policy = CREATE_IF_DOESNT_EXIST);
    
    std::string get(const std::string& key);

    // Sets a new value and returns the old string in a single operation
    std::string getset(const std::string& key, const std::string& value);

    int append(const std::string& key, const std::string& value);
    
    std::string getRange(const std::string& key, int start, int end);
    
    int incrBy(const std::string& key, int by);

    std::string incrBy(const std::string& key, double by);

    // Returns all values under provided keys
    std::vector<std::pair<std::string, int>> mget(const std::vector<std::string>& keys);

    // Returns keys and values
    std::vector<std::pair<std::string, int>> getAll(int getAllType);
    
    // Returns number of all keys stored in the databases
    virtual int size();
    
    // Flushes all data
    virtual void flush();
    
    // Finds where the given key is and deletes its content
    virtual int delKey(const std::string& key);
    
    // Finds where the given key is and deletes its content
    virtual bool keyExists(const std::string& key);
    
    // Returns the container type associated with key
    IMapCommon::ContainerType getContainerType();

    // Renames the key to newkey
    virtual int rename(const std::string& key, const std::string& newKey);
    
    // Scans members starting from a given cursor
    int scan(std::vector<std::pair<std::string, int>>& outArr,
             const std::string& pattern,
             int cursor = 0,
             int timestamp = 0,
             int upperLimit = INT_MAX);
    
    void cleanup();
    
    int getCounter() const;
    
private:
    
    friend class SelfTest;
    
    std::unordered_map<std::string, std::string> map;
    // Currently the implemention is thread safe and uses a lock for it. However given
    // that there is only one processing thread - we might need to remove it as the
    // lock is not needed.
    std::recursive_mutex mLock;
    int counter;
};
    
}; // __arag__StringMap__


#endif /* defined(__CacheServer__Map__) */
