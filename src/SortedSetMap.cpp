#include "SortedSetMap.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <iostream>
#include <cfloat>

using namespace std;
using namespace arag;
using namespace goodliffe;

static int bound(const SortedSetMap::SortedSet::SkipListType& l,
                int first,
                int last,
                function<bool(const SortedSetMap::Item&)> fComp)
{
    int it;
    int count, step;
    
    count = last - first;
    
    while (count > 0) {
        it = first;
        step = count / 2;
        it += step;
        
        if (fComp(l[it])) {
            first = ++it;
            count -= step + 1;
        }
        else {
            count = step;
        }
    }
    return first;
}

static int bound(const SortedSetMap::SortedSet::SkipListType& l,
                       int first,
                       int last,
                       bool bDVal,
                       double dval,
                       string sval,
                       SortedSetMap::Bound direction)
{
    function<bool(const SortedSetMap::Item&)> fComp;
    
    fComp = [bDVal, dval, sval, direction] (const SortedSetMap::Item& item) {
        if (bDVal) {
            if ((direction == SortedSetMap::Bound::LOWER_BOUND && item.score < dval) ||
                (direction == SortedSetMap::Bound::UPPER_BOUND && !(dval < item.score))) {
                return true;
            }
        }
        else {
            if ((direction == SortedSetMap::Bound::LOWER_BOUND && (item.val.compare(sval) < 0)) ||
                (direction == SortedSetMap::Bound::UPPER_BOUND && !((sval.compare(item.val) < 0)))) {
                return true;
            }
        }
        return false;
    };
    
    return bound(l, first, last, fComp);
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
    
    if (sset.mMap.size() == 0) {
        return ret;
    }
        
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
    
    int lower = 0;
    if (min != DBL_MIN) {
        lower = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), true, min, "", LOWER_BOUND);
    }
    
    int upper = (int)sset.mSkipList.size();
    if (max != DBL_MAX) {
        upper = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), true, max, "", UPPER_BOUND);
    }
    
    return (int)(upper - lower);
}

int SortedSetMap::lexCount(const std::string &key, const string& min, const string& max)
{
    SortedSet& sset = mSetMap[key];
    
    int lower = 0;
    if (min[0] != char(-127)) {
        lower = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), false, 0, min, LOWER_BOUND);
    }
    
    int upper = (int)sset.mSkipList.size();
    if (max[0] != char(127)) {
        upper = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), false, 0, max, UPPER_BOUND);
    }
    
    return (int)(upper - lower);
}

SortedSetMap::RedisArray SortedSetMap::rangeByScore(const std::string &key,
                                                    double min,
                                                    double max,
                                                    int offset,
                                                    int count,
                                                    bool bWithScores,
                                                    bool bReverse)
{
    SortedSet& sset = mSetMap[key];
    
    RedisArray ret;
    
    int lower = 0;
    if (min != DBL_MIN) {
        lower = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), true, min, "", LOWER_BOUND);
    }
    
    int upper = (int)sset.mSkipList.size();
    if (max != DBL_MAX) {
        upper = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), true, max, "", UPPER_BOUND);
    }
    
    if (count < upper - lower) {
        if (bReverse) {
            lower = upper - count;
        }
        else {
            upper = lower + count;
        }
    }
    
    function<void(RedisArray&, const Item&, bool)> fAddToArray;
    // A function that adds items to RedisArray
    fAddToArray = [] (RedisArray& ret, const Item& item, bool bWithScores) {
        ret.push_back(make_pair(item.val, RedisProtocol::DataType::BULK_STRING));
        if (bWithScores) {
            ret.push_back(make_pair(Utils::dbl2str(item.score), RedisProtocol::DataType::BULK_STRING));
        }
    };
    
    if (!bReverse) {
        for (int i = offset + lower; i < upper; ++i) {
            fAddToArray(ret, sset.mSkipList[i], bWithScores);
        }
    }
    else {
        for (int i = upper - offset - 1; i >= lower; --i) {
            fAddToArray(ret, sset.mSkipList[i], bWithScores);
        }
    }
    return ret;
}

SortedSetMap::RedisArray SortedSetMap::rangeByLex(const std::string &key,
                                                  const std::string& min,
                                                  const std::string& max,
                                                    int offset,
                                                    int count,
                                                    bool bReverse)
{
    SortedSet& sset = mSetMap[key];
    
    RedisArray ret;
    
    int lower = 0;
    if (min[0] != CHAR_MIN) {
        lower = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), false, 0, min, LOWER_BOUND);
    }
    
    int upper = (int)sset.mSkipList.size();
    if (max[0] != CHAR_MAX) {
        upper = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), false, 0, max, UPPER_BOUND);
    }
    
    if (count < upper - lower) {
        if (bReverse) {
            lower = upper - count;
        }
        else {
            upper = lower + count;
        }
    }
    
    function<void(RedisArray&, const Item&, bool)> fAddToArray;
    // A function that adds items to RedisArray
    fAddToArray = [] (RedisArray& ret, const Item& item, bool bWithScores) {
        ret.push_back(make_pair(item.val, RedisProtocol::DataType::BULK_STRING));
    };
    
    if (!bReverse) {
        for (int i = offset + lower; i < upper; ++i) {
            fAddToArray(ret, sset.mSkipList[i], false);
        }
    }
    else {
        for (int i = upper - offset - 1; i >= lower; --i) {
            fAddToArray(ret, sset.mSkipList[i], false);
        }
    }
    return ret;
}

int SortedSetMap::size()
{
    return (int)(mSetMap.size());
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

int SortedSetMap::intersect(const string& destKey,
                         const vector<string>& keys,
                         const vector<int>& weights,
                         const string& aggregate)
{
    SortedSet tempSS = mSetMap[keys[0]];
    SortedSet::MapType& tempMap = tempSS.mMap;
    
    bool erased = false;
    auto iter = tempMap.begin();
    while (iter != tempMap.end()) {
        erased = false;
        for (int i = 1; i < keys.size(); ++i) {
            SortedSet::MapType& nextMap = mSetMap[keys[i]].mMap;
            
            auto elem = nextMap.find(iter->first);
            if (elem == nextMap.end()) {
                iter = tempMap.erase(iter);
                erased = true;
                break;
            }

            // Item found, need to update with Weights and Aggregation
            double newVal = elem->second * weights[i];
            double& curVal = tempMap[elem->first];
            curVal *= weights[0];
            if (aggregate == "SUM") {
                curVal += newVal;
            }
            else
            if (aggregate == "MIN") {
                curVal = min(newVal, curVal);
            }
            else
            if (aggregate == "MAX") {
                curVal = max(newVal, curVal);
            }
        }
        if (!erased) {
            iter++;
        }
    }
    
    mSetMap[destKey].mMap = tempSS.mMap;
    
    SortedSet& destSS = mSetMap[destKey];
    
    // Insert all elements into Skip List
    for (auto elem : destSS.mMap) {
        destSS.mSkipList.insert(Item(elem.first, elem.second));
    }
    
    return (int)destSS.mMap.size();
}

int SortedSetMap::uni(const string& destKey,
                      const vector<string>& keys,
                      const vector<int>& weights,
                      const string& aggregate)
{
    SortedSet& destSet = mSetMap[destKey];
    
    destSet = SortedSet();
    
    for (int i = 0; i < keys.size(); ++i) {
        SortedSet& ss = mSetMap[keys[i]];
        
        // Copy all elements in the map
        for (auto elem : ss.mMap) {
            double newVal = elem.second * weights[i];
            if (aggregate == "SUM") {
                destSet.mMap[elem.first] += newVal;
            }
            else
            if (aggregate == "MIN") {
                double oldVal = INT_MAX;
                if (destSet.mMap.find(elem.first) != destSet.mMap.end()) {
                    oldVal = destSet.mMap[elem.first];
                }
                destSet.mMap[elem.first] = min(newVal, oldVal);
            }
            else
            if (aggregate == "MAX") {
                double oldVal = INT_MIN;
                if (destSet.mMap.find(elem.first) != destSet.mMap.end()) {
                    oldVal = destSet.mMap[elem.first];
                }
                destSet.mMap[elem.first] = max(newVal, oldVal);
            }
        }
    }

    // Insert all elements into Skip List
    for (auto elem : destSet.mMap) {
        destSet.mSkipList.insert(Item(elem.first, elem.second));
    }
    
    return (int)destSet.mMap.size();
}

int SortedSetMap::remByRank(const std::string &key, int start, int stop)
{
    SortedSet& sset = mSetMap[key];

    auto first = sset.mSkipList.iterator_at(start);
    auto last = sset.mSkipList.iterator_at(stop);
    int num = (int)std::distance(first, last) + 1;
    
    for (auto iter = first; iter != last + 1; ++iter) {
        sset.mMap.erase((*iter).val);
    }
    
    if (first == sset.mSkipList.begin() && last == sset.mSkipList.end()) {
        sset.mSkipList.clear();
    }
    else {
        sset.mSkipList.erase(first, last + 1);
    }
    
    return num;
}

int SortedSetMap::remByScore(const std::string &key, double min, double max)
{
    SortedSet& sset = mSetMap[key];

    int lower = 0;
    if (min != DBL_MIN) {
        lower = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), true, min, "", LOWER_BOUND);
    }
    
    int upper = (int)sset.mSkipList.size();
    if (max != DBL_MAX) {
        upper = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), true, max, "", UPPER_BOUND);
    }

    return remByRank(key, lower, upper - 1);
}


int SortedSetMap::remByLex(const std::string &key, const std::string &min, const std::string &max)
{
    SortedSet& sset = mSetMap[key];
    
    int lower = 0;
    if (min[0] != CHAR_MIN) {
        lower = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), false, 0, min, LOWER_BOUND);
    }
    
    int upper = (int)sset.mSkipList.size();
    if (max[0] != CHAR_MAX) {
        upper = bound(sset.mSkipList, 0, (int)sset.mSkipList.size(), false, 0, max, UPPER_BOUND);
    }
    
    return remByRank(key, lower, upper - 1);
}


void SortedSetMap::clearKeys()
{
    mSetMap.clear();
}

int SortedSetMap::delKey(const std::string &key)
{
    auto iter = mSetMap.find(key);
    if (iter == mSetMap.end()) {
        return 0;
    }

    SortedSet& sset = iter->second;
    sset.mSkipList.clear();
    
    mSetMap.erase(iter);
    return 1;
}
