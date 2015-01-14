#include "InMemoryData.h"
#include "Config.h"

using namespace arag;
using namespace std;

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

void InMemoryData::cleanup()
{
    mStringMap.cleanup();
    
    for (auto map = mHashMap.begin(); map != mHashMap.end(); ++map) {
        map->second.cleanup();
    }
}

void InMemoryData::flush()
{
    mStringMap.clearKeys();
    mSetMap.clearKeys();
    mSortedSetMap.clearKeys();
    mListMap.clearKeys();
    mHashMap.clear();
    mHLLMap.clearKeys();
}

int InMemoryData::size()
{
    return (int)(mStringMap.size() + mSetMap.size() +
            mSortedSetMap.size() + mListMap.size() + mHashMap.size() + mHLLMap.size());
}

int InMemoryData::delKey(const std::string &key)
{
    return mStringMap.delKey(key) +
            mSetMap.delKey(key) +
            mSortedSetMap.delKey(key) +
            mListMap.delKey(key) +
            mHLLMap.delKey(key) +
            (int)mHashMap.erase(key);
}

InMemoryData::ContainerType InMemoryData::getKeyType(const std::string &key)
{
    if (mStringMap.keyExists(key)) {
        return ContainerType::STRING;
    }

    if (mSetMap.keyExists(key)) {
        return ContainerType::SET;
    }

    if (mSortedSetMap.keyExists(key)) {
        return ContainerType::SORTEDSET;
    }

    if (mListMap.keyExists(key)) {
        return ContainerType::LIST;
    }

    if ((mHashMap.find(key) != mHashMap.end())) {
        return ContainerType::HASH;
    }

    if (mHLLMap.keyExists(key)) {
        return ContainerType::HLL;
    }
    
    return ContainerType::NONE;
}

bool InMemoryData::keyExists(const std::string& key)
{
    bool bExists = mStringMap.keyExists(key) || mSetMap.keyExists(key) ||
    mSortedSetMap.keyExists(key) || mListMap.keyExists(key) ||
    (mHashMap.find(key) != mHashMap.end());
    
    return bExists;
}

void InMemoryData::addSubscriber(const std::string& channel, int sid, bool pattern)
{
    PubSubElement& elem = mSubscrMap[channel];
    elem.first.push_back(sid);
    elem.second.first = pattern;
    elem.second.second = channel;
}

std::list<InMemoryData::PubSubElement> InMemoryData::getSubscribers(const std::string& channel)
{
    std::list<InMemoryData::PubSubElement> subscrs;
    
    for (auto iter = mSubscrMap.begin(); iter != mSubscrMap.end(); ++iter) {
        string ch = iter->first;
        if (Utils::checkPattern(channel, iter->first)) {
            subscrs.push_back(iter->second);
        }
    }
    
    return subscrs;
}

void InMemoryData::removeSubscriber(const std::string& channel, int sid)
{
    auto elem = mSubscrMap.find(channel);
    if (elem == mSubscrMap.end()) {
        return;
    }
    
    elem->second.first.remove(sid);
}

vector<string> InMemoryData::unsubscribeFromAllChannels(int sid)
{
    vector<string> chnls;
    for (auto iter = mSubscrMap.begin(); iter != mSubscrMap.end(); ++iter) {
        const std::list<int>& l = iter->second.first;
        auto elem = std::find(l.begin(), l.end(), sid);
        if (elem == l.end()) {
            continue;
        }
        chnls.push_back(iter->first);
        iter->second.first.remove(sid);
    }
    
    return chnls;
}

void InMemoryData::removeSubscriber(const std::vector<std::string>& patterns, int sid)
{
    for (auto elem : mSubscrMap) {
        std::string channel = elem.first;
        for (auto patt : patterns) {
            if (Utils::checkPattern(channel, patt)) {
                removeSubscriber(channel, sid);
            }
        }
    }
}

int InMemoryData::getSubscribersNum(int sid)
{
    int num = 0;
    for (auto iter = mSubscrMap.begin(); iter != mSubscrMap.end(); ++iter) {
        const std::list<int>& l = iter->second.first;
        auto elem = std::find(l.begin(), l.end(), sid);
        if (elem != l.end()) {
            num++;
        }
    }
    
    return num;
}

//---------------------------------------------------

Database& Database::instance()
{
    static Database sDB(Config::DATABASE_COUNT);
    return sDB;
}

Database::Database(int count)
{
    mDatabases = vector<InMemoryData>(count);
}

InMemoryData& Database::get(int index)
{
    if (index >= mDatabases.size()) {
        throw invalid_argument("Index is bigger than number of DBs");
    }
    
    return mDatabases[index];
}

void Database::flush(int index)
{
    if (index >= mDatabases.size()) {
        throw invalid_argument("Index is bigger than number of DBs");
    }

    if (index == FLUSH_ALL) {
        for (int i = 0; i < mDatabases.size(); ++i) {
            mDatabases[i].flush();
        }
    }
    else {
        mDatabases[index].flush();
    }
}
