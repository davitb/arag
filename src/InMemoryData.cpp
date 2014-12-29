#include "InMemoryData.h"

using namespace arag;

int InMemoryData::getCounter() const
{
    int c = mStringMap.getCounter();
    
    for (auto map = mHashMap.begin(); map != mHashMap.end(); ++map) {
        c += map->second.getCounter();
    }
    
    return c;
}

StringMap& InMemoryData::getFromHashMap(const std::string& key)
{
    return mHashMap[key];
};

StringMap& InMemoryData::getStringMap()
{
    return mStringMap;
};

void InMemoryData::cleanup()
{
    mStringMap.cleanup();
    
    for (auto map = mHashMap.begin(); map != mHashMap.end(); ++map) {
        map->second.cleanup();
    }
}
