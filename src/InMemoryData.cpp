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

StringMap& InMemoryData::getStringMap()
{
    return mStringMap;
};

ListMap& InMemoryData::getListMap()
{
    return mListMap;
};

SetMap& InMemoryData::getSetMap()
{
    return mSetMap;
};

SortedSetMap& InMemoryData::getSortedSetMap()
{
    return mSortedSetMap;
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
}

int InMemoryData::size()
{
    return (int)(mStringMap.size() + mSetMap.size() +
            mSortedSetMap.size() + mListMap.size() + mHashMap.size());
}

int InMemoryData::delKey(const std::string &key)
{
    return mStringMap.deleteKey(key) +
            mSetMap.delKey(key) +
            mSortedSetMap.delKey(key) +
            mListMap.delKey(key) +
            (int)mHashMap.erase(key);
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
