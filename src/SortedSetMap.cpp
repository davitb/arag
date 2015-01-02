#include "SortedSetMap.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <iostream>

using namespace std;
using namespace arag;
using namespace goodliffe;

static int bound(const SortedSetMap::SortedSet::SkipListType& l,
                int first,
                int last,
                const double& value,
                bool direction)
{
    int it;
    int count, step;
    
    count = last - first;
    
    while (count > 0) {
        it = first;
        step = count / 2;
        it += step;
        if ((direction == false && l[it].score < value) || (direction && !(value < l[it].score))) {
            first = ++it;
            count -= step + 1;
        } else count = step;
    }
    return first;
}

int SortedSetMap::insert(const std::string &key, const std::string &val, double score)
{
    SortedSet& sset = mSetMap[key];
    
    auto iter = sset.mMap.find(val);
    if (iter == sset.mMap.end()) {
        auto result = sset.mSkipList.insert(Item(val, score));
        if (!result.second) {
            throw invalid_argument("Cannot add new elem");
        }
        sset.mMap[val] = score;
        return 1;
    }
    
    // Nothing to do if we are updating to the same score
    if (iter->second == score) {
        return 0;
    }

    sset.mSkipList.erase(Item(val, iter->second));
    auto result = sset.mSkipList.insert(Item(val, score));
    if (!result.second) {
        throw invalid_argument("Cannot add new elem");
    }
    // Update the map
    iter->second = score;

    return 0;
}

SortedSetMap::RedisArray SortedSetMap::range(const std::string &key, int start, int end, bool bWithScores)
{
    SortedSet& sset = mSetMap[key];
    
    RedisArray ret;
        
    for (int i = start; i <= end; ++i) {
        const Item& item = sset.mSkipList[i];
        ret.push_back(make_pair(item.val, RedisProtocol::DataType::BULK_STRING));
        if (bWithScores) {
            ret.push_back(make_pair(Utils::dbl2str(item.score), RedisProtocol::DataType::BULK_STRING));
        }
    }
    
    return ret;
}

SortedSetMap::RedisArray SortedSetMap::revRange(const std::string &key, int start, int end, bool bWithScores)
{
    SortedSet& sset = mSetMap[key];
    
    RedisArray ret;
    
    int len = (int)sset.mSkipList.size();
    start = len - start - 1;
    end = len - end - 1;
    
    for (int i = start; i >= end; --i) {
        const Item& item = sset.mSkipList[i];
        ret.push_back(make_pair(item.val, RedisProtocol::DataType::BULK_STRING));
        if (bWithScores) {
            ret.push_back(make_pair(Utils::dbl2str(item.score), RedisProtocol::DataType::BULK_STRING));
        }
    }
    
    return ret;
}

double SortedSetMap::score(const std::string &key, const std::string &member)
{
    SortedSet& sset = mSetMap[key];
    
    auto iter = sset.mMap.find(member);
    if (iter == sset.mMap.end()) {
        throw invalid_argument("Wrong member");
    }
    
    return sset.mMap[member];
}

int SortedSetMap::rank(const std::string &key, const std::string &member, bool reverse)
{
    SortedSet& sset = mSetMap[key];
    
    auto iter = sset.mMap.find(member);
    if (iter == sset.mMap.end()) {
        throw invalid_argument("Wrong member");
    }
    
    double score = iter->second;
    if (!reverse) {
        return (int)std::distance(sset.mSkipList.begin(), sset.mSkipList.find(Item(member, score)));
    }
    return (int)std::distance(sset.mSkipList.find(Item(member, score)), sset.mSkipList.end()) - 1;
}

int SortedSetMap::count(const std::string &key, double min, double max)
{
    SortedSet& sset = mSetMap[key];
    
    int lower = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), min, false);
    int upper = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), max, true);
    
    return (int)(upper - lower);
}

int SortedSetMap::size(const std::string &key)
{
    SortedSet& sset = mSetMap[key];
    
    return (int)(sset.mMap.size());
}

double SortedSetMap::incrBy(const std::string &key, const std::string &member, double by)
{
    SortedSet& sset = mSetMap[key];

    auto iter = sset.mMap.find(member);
    if (iter == sset.mMap.end()) {
        insert(key, member, by);
        return by;
    }
    
    double newScore = iter->second + by;
    insert(key, member, newScore);
    
    return newScore;
}

int SortedSetMap::rem(const std::string &key, const std::string &val)
{
    SortedSet& sset = mSetMap[key];
    
    auto iter = sset.mMap.find(val);
    if (iter == sset.mMap.end()) {
        return 0;
    }
    
    sset.mSkipList.erase(Item(val, iter->second));
    sset.mMap.erase(iter);
    return 1;
}

