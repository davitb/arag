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

/*
    This class exposes all Redis data structures to Commands.
    Every data structure is implemented as a map which supports
    certain functionality. All maps implement IMapCommon interface.
 */
class InMemoryData : IMapCommon
{
public:

    typedef std::unordered_map<std::string, StringMap> HashMap;
        
    InMemoryData();
    
    StringMap& getStringMap() { return mStringMap; }
    
    // Returns the StringMap associated with given key
    StringMap& getFromHashMap(const std::string& key);
    
    ListMap& getListMap() { return mListMap; }
    
    SetMap& getSetMap() { return mSetMap; }
    
    SortedSetMap& getSortedSetMap() { return mSortedSetMap; }

    HLLMap& getHyperLogLogMap() { return mHLLMap; }

    PubSubMap& getPubSubMap() { return mPubSubMap; }

    ScriptMap& getScriptMap() { return mScriptMap; }
    
    // Returns number of all keys stored in the databases
    virtual int size();

    // Flushes all data
    virtual void flush();
    
    // Finds where the given key is and deletes its content
    virtual int delKey(const std::string& key);

    // Finds where the given key is and deletes its content
    virtual bool keyExists(const std::string& key);

    // Returns the container type associated with key
    IMapCommon::ContainerType getContainerType(const std::string& key);

private:
    
    // Returns ContainerType::NONE
    IMapCommon::ContainerType getContainerType();
    
private:
    
    StringMap mStringMap;
    ListMap mListMap;
    HashMap mHashMap;
    SetMap mSetMap;
    SortedSetMap mSortedSetMap;
    PubSubMap mPubSubMap;
    HLLMap mHLLMap;
    ScriptMap mScriptMap;
    std::vector<std::reference_wrapper<IMapCommon>> mCommonMaps;
};

};

#endif /* defined(__arag__InMemoryData__) */
