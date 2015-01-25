#ifndef __arag__HLLMap__
#define __arag__HLLMap__

#include <unordered_map>
#include <string>
#include <vector>
#include "hyperloglog.hpp"
#include "IMapCommon.h"

namespace arag
{
    
/*
    This class implements a map for mapping keys to HyperLogLog elements.
 */
class HLLMap : public IMapCommon
{
private:

    enum
    {
        BIT_WIDTH = 10
    };
    
public:
    typedef std::unordered_map<std::string, hll::HyperLogLog> HLLMapType;
    
    int add(const std::string &key, const std::string& val);
    
    // Returns cardinality of HyperLogLog stored under key
    int count(const std::string &key);

    // Returns cardinality of the union of HyperLogLogs stored under keys
    int count(const std::vector<std::string> &keys);

    // Merges HyperLogLogs from keys into destKey
    void merge(const std::string& destKey, const std::vector<std::string> &keys);
    
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
    
private:
    
    HLLMapType mHLLMap;
};

};



#endif /* defined(__arag__HLLMap__) */
