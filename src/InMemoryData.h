#ifndef __arag__InMemoryData__
#define __arag__InMemoryData__

#include <unordered_map>
#include <string>
#include "StringMap.h"
#include "ListMap.h"
#include "SetMap.h"

namespace arag
{
    
class InMemoryData
{
public:
    typedef std::unordered_map<std::string, StringMap> HashMap;
    
    StringMap& getFromHashMap(const std::string& key);
    
    StringMap& getStringMap();

    ListMap& getListMap();
    
    SetMap& getSetMap();
    
    int getCounter() const;
    
    void cleanup();
    
private:
    
    StringMap mStringMap;
    ListMap mListMap;
    HashMap mHashMap;
    SetMap mSetMap;
    int mCounter;
};

};

#endif /* defined(__arag__InMemoryData__) */
