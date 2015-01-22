#ifndef __arag__InMemoryData__
#define __arag__InMemoryData__

#include <unordered_map>
#include <string>
#include <vector>
#include <list>
#include "AragException.h"
#include "StringMap.h"
#include "ListMap.h"
#include "SetMap.h"
#include "SortedSetMap.h"
#include "HLLMap.h"
#include "PubSubMap.h"
#include "ScriptMap.h"
#include "Utils.h"
#include "EventPublisher.h"
#include <iostream>

namespace arag
{

class InMemoryData
{
public:

    typedef std::unordered_map<std::string, StringMap> HashMap;
    
    enum ContainerType
    {
        NONE,
        STRING,
        HASH,
        LIST,
        SET,
        SORTEDSET,
        HLL
    };
    
    
    StringMap& getFromHashMap(const std::string& key);
    
    StringMap& getStringMap() { return mStringMap; }
    
    ListMap& getListMap() { return mListMap; }
    
    SetMap& getSetMap() { return mSetMap; }
    
    SortedSetMap& getSortedSetMap() { return mSortedSetMap; }

    HLLMap& getHyperLogLogMap() { return mHLLMap; }

    PubSubMap& getPubSubMap() { return mPubSubMap; }

    ScriptMap& getScriptMap() { return mScriptMap; }
    
    int size();
    
    int getCounter() const;
    
    void cleanup();
    
    void flush();
    
    int delKey(const std::string& key);
    
    bool keyExists(const std::string& key);
    
    ContainerType getKeyType(const std::string& key);
    
private:
    
    StringMap mStringMap;
    ListMap mListMap;
    HashMap mHashMap;
    SetMap mSetMap;
    SortedSetMap mSortedSetMap;
    PubSubMap mPubSubMap;
    HLLMap mHLLMap;
    ScriptMap mScriptMap;
    int mCounter;
};

};

#endif /* defined(__arag__InMemoryData__) */
