#include "InMemoryData.h"

using namespace arag;
using namespace std;

InMemoryData::InMemoryData()
{
    mCommonMaps.push_back(mStringMap);
    mCommonMaps.push_back(mSetMap);
    mCommonMaps.push_back(mSortedSetMap);
    mCommonMaps.push_back(mListMap);
    mCommonMaps.push_back(mHLLMap);
}

StringMap& InMemoryData::getFromHashMap(const std::string& key)
{
    return mHashMap[key];
};

void InMemoryData::flush()
{
    for (int i = 0; i < mCommonMaps.size(); ++i) {
        mCommonMaps[i].get().flush();
    }
    mHashMap.clear();
}

int InMemoryData::size()
{
    int num = 0;
    for (int i = 0; i < mCommonMaps.size(); ++i) {
        num += mCommonMaps[i].get().size();
    }
    
    return (int)(num + mHashMap.size());
}

int InMemoryData::delKey(const std::string &key)
{
    int num = 0;
    for (int i = 0; i < mCommonMaps.size(); ++i) {
        num += mCommonMaps[i].get().delKey(key);
    }
    
    return num + (int)mHashMap.erase(key);
}

IMapCommon::ContainerType InMemoryData::getContainerType()
{
    return IMapCommon::ContainerType::NONE;
}

IMapCommon::ContainerType InMemoryData::getContainerType(const std::string &key)
{
    for (int i = 0; i < mCommonMaps.size(); ++i) {
        if (mCommonMaps[i].get().keyExists(key)) {
            return mCommonMaps[i].get().getContainerType();
        }
    }
    
    if ((mHashMap.find(key) != mHashMap.end())) {
        return IMapCommon::HASH;
    }
    
    return IMapCommon::NONE;
}

bool InMemoryData::keyExists(const std::string& key)
{
    for (int i = 0; i < mCommonMaps.size(); ++i) {
        if (mCommonMaps[i].get().keyExists(key)) {
            return true;
        }
    }
    
    if (mHashMap.count(key) > 0) {
        return true;
    }
    
    return false;
}

