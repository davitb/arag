#ifndef __arag__InMemoryData__
#define __arag__InMemoryData__

#include <unordered_map>
#include <string>
#include <vector>
#include <list>
#include "StringMap.h"
#include "ListMap.h"
#include "SetMap.h"
#include "SortedSetMap.h"
#include "HLLMap.h"
#include "Utils.h"
#include <iostream>

namespace arag
{

class InMemoryData
{
public:

    typedef std::unordered_map<std::string, StringMap> HashMap;
    typedef std::pair<std::list<int>, std::pair<bool, std::string>> PubSubElement;
    typedef std::unordered_map<std::string, PubSubElement> SubscribersMap;
    
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

    void addSubscriber(const std::string& channel, int sid, bool pattern = false);

    std::list<PubSubElement> getSubscribers(const std::string& channel);
    
    void removeSubscriber(const std::string& channel, int sid);
    
    std::vector<std::string> unsubscribeFromAllChannels(int sid);
    
    void removeSubscriber(const std::vector<std::string>& patterns, int sid);
    
    int getSubscribersNum(int sid);
    
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
    SubscribersMap mSubscrMap;
    HLLMap mHLLMap;
    int mCounter;
};

    
    
class Database
{
public:

    enum
    {
        FLUSH_ALL = -1
    };
    
    static Database& instance();
    
    Database(int count);
    
    InMemoryData& get(int index);
    
    void flush(int index);
    
private:
    std::vector<InMemoryData> mDatabases;
};


};

#endif /* defined(__arag__InMemoryData__) */
