#include "HLLMap.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <iostream>

using namespace std;
using namespace arag;
using namespace hll;

int HLLMap::add(const std::string &key, const std::string &val)
{
    if (!keyExists(key)) {
        mHLLMap[key] = HyperLogLog(BIT_WIDTH);
    }
    
    return mHLLMap[key].add(val.c_str(), (int)val.length());
}

int HLLMap::count(const std::string &key)
{
    if (!keyExists(key)) {
        return 0;
    }
    
    return mHLLMap[key].estimate();
}

void HLLMap::merge(const std::string& destKey, const std::vector<std::string> &keys)
{
    if (!keyExists(destKey)) {
        mHLLMap[destKey] = HyperLogLog(BIT_WIDTH);
    }
    
    for (auto key : keys) {
        mHLLMap[destKey].merge(mHLLMap[key]);
    }
}

int HLLMap::count(const std::vector<std::string> &keys)
{
    HyperLogLog hll(BIT_WIDTH);

    for (auto key : keys) {
        hll.merge(mHLLMap[key]);
    }
    
    return hll.estimate();
}

void HLLMap::flush()
{
    mHLLMap.clear();
}

int HLLMap::delKey(const std::string &key)
{
    auto iter = mHLLMap.find(key);
    if (iter == mHLLMap.end()) {
        return 0;
    }
    
    mHLLMap.erase(iter);
    return 1;
}

bool HLLMap::keyExists(const std::string &key)
{
    return mHLLMap.find(key) != mHLLMap.end();
}

int HLLMap::size()
{
    return (int)mHLLMap.size();
}

IMapCommon::ContainerType HLLMap::getContainerType()
{
    return IMapCommon::HLL;
}
