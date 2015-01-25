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
#include "KeysMap.h"
#include "HashMap.h"
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
class InMemoryData
{
public:

    InMemoryData();
    
    StringMap& getStringMap() { return mStringMap; }
    
    ListMap& getListMap() { return mListMap; }
    
    SetMap& getSetMap() { return mSetMap; }
    
    SortedSetMap& getSortedSetMap() { return mSortedSetMap; }

    HLLMap& getHyperLogLogMap() { return mHLLMap; }

    PubSubMap& getPubSubMap() { return mPubSubMap; }

    ScriptMap& getScriptMap() { return mScriptMap; }

    KeyMap& getKeyMap() { return _keyMap; }

    HashMap& getHashMap() { return _hashMap; }
    
private:
    
    StringMap mStringMap;
    ListMap mListMap;
    HashMap mHashMap;
    SetMap mSetMap;
    SortedSetMap mSortedSetMap;
    PubSubMap mPubSubMap;
    HLLMap mHLLMap;
    ScriptMap mScriptMap;
    KeyMap _keyMap;
    HashMap _hashMap;
};

};

#endif /* defined(__arag__InMemoryData__) */
