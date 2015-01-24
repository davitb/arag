#ifndef __arag__HashMap__
#define __arag__HashMap__

#include <string>
#include <unordered_map>
#include "StringMap.h"

namespace arag
{

/*
 This class implements a map to store all keys and related inforation.
 */
class HashMap : public IMapCommon
{
public:

    typedef std::unordered_map<std::string, StringMap> HashMapType;
    
    virtual int size();
    
    // Delete the specified key
    virtual int delKey(const std::string& key);
    
    // Checks if the specified key exists
    virtual bool keyExists(const std::string& key);
    
    // Get container type - returns NONE
    virtual IMapCommon::ContainerType getContainerType();
    
    // Clears all keys
    virtual void flush();
    
    // Return value under the key
    StringMap& get(const std::string& key);
    
private:
    
    HashMapType _hashMap;
};

};

#endif /* defined(__arag__HashMap__) */
