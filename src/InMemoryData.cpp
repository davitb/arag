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
        throw EWrongDBIndex();
    }
    
    return mDatabases[index];
}

void Database::flush(int index)
{
    if (index >= mDatabases.size()) {
        throw EWrongDBIndex();
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
