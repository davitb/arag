#include <iterator>
#include "SetMap.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <iostream>

using namespace std;
using namespace arag;

int SetMap::size()
{
    return (int)mSetMap.size();
}

int SetMap::size(const std::string &key)
{
    auto iter = mSetMap.find(key);
    if (iter == mSetMap.end()) {
        return 0;
    }
    
    return (int)mSetMap[key].size();
}

int SetMap::add(const std::string &key, const std::string &val)
{
    auto iter = mSetMap[key].insert(val);
    if (iter.second) {
        return 1;
    }
    
    return 0;
}

int SetMap::rem(const std::string &key, const std::string &val)
{
    return (int)mSetMap[key].erase(val);
}

int SetMap::isMember(const std::string &key, const std::string &val)
{
    auto iter = mSetMap[key].find(val);
    if (iter == mSetMap[key].end()) {
        return 0;
    }
    
    return 1;
}

vector<pair<string, int>> SetMap::getMembers(const string& key)
{
    vector<pair<string, int>> members;
    
    unordered_set<string>& s = mSetMap[key];
    
    for (auto iter = s.begin(); iter != s.end(); ++iter) {
        members.push_back(make_pair(*iter, RedisProtocol::DataType::BULK_STRING));
    }
    
    return members;
}

string SetMap::getRandMember(const std::string &key)
{
    if (size(key) == 0) {
        throw invalid_argument("Wrong key");
    }

    unordered_set<string>& s = mSetMap[key];
    int rand = Utils::genRandom(0, (int)s.size() - 1);
    auto iter = s.begin();
    std::advance(iter, rand);
    return *iter;
}

vector<pair<string, int>> SetMap::getRandMembers(const std::string &key, const int n)
{
    if (size(key) == 0) {
        return vector<pair<string, int>>();
    }

    SetType& s = mSetMap[key];

    int size = (int)s.size();
    vector<pair<string, int>> res(min(n, size));
    if (n >= size) {
        auto iter = s.begin();
        for (int i = 0; i < size; ++i) {
            res[i] = make_pair(*iter++, RedisProtocol::DataType::BULK_STRING);
        }
        return res;
    }
    
    int rand = Utils::genRandom(0, size - n);
    auto iter = s.begin();
    std::advance(iter, rand);
    
    for (int i = 0; i < n; ++i) {
        res[i] = make_pair(*iter++, RedisProtocol::DataType::BULK_STRING);
    }
    
    return res;
}

void SetMap::diff(const string& key, const vector<string>& diffKeys, SetType& destSet)
{
    SetType& sourceSet = mSetMap[key];
    
    destSet = sourceSet;
    
    for (int i = 0; i < diffKeys.size(); ++i) {
        SetType& s = mSetMap[diffKeys[i]];
        
        for (auto val : s) {
            destSet.erase(val);
        }
    }
}

int SetMap::diff(const string& destKey, const string& key, const vector<string>& diffKeys)
{
    diff(key, diffKeys, mSetMap[destKey]);
    
    return size(destKey);
}

void SetMap::inter(const vector<string>& keys, SetType& destSet)
{
    destSet = mSetMap[keys[0]];

    bool erased = false;
    auto iter = destSet.begin();
    while (iter != destSet.end()) {
        erased = false;
        for (int i = 1; i < keys.size(); ++i) {
            SetType& s = mSetMap[keys[i]];
            
            if (s.find(*iter) == s.end()) {
                iter = destSet.erase(iter);
                erased = true;
                break;
            }
        }
        if (!erased) {
            iter++;
        }
    }
}

int SetMap::inter(const string& destKey, const vector<string>& keys)
{
    inter(keys, mSetMap[destKey]);
    
    return size(destKey);
}

void SetMap::uni(const vector<string>& keys, SetType& destSet)
{
    for (int i = 0; i < keys.size(); ++i) {
        SetType& s = mSetMap[keys[i]];
        destSet.insert(s.begin(), s.end());
    }
}

int SetMap::uni(const string& destKey, const vector<string>& keys)
{
    uni(keys, mSetMap[destKey]);
    
    return size(destKey);
}

int SetMap::move(const string& source, const string& dest, const string& member)
{
    if (mSetMap.find(dest) == mSetMap.end() || mSetMap.find(source) == mSetMap.end()) {
        return 0;
    }
    
    SetType& sourceSet = mSetMap[source];
    auto iter = sourceSet.find(member);
    if (iter == sourceSet.end()) {
        return 0;
    }
    
    sourceSet.erase(member);
    
    mSetMap[dest].insert(member);
    return 1;
}

void SetMap::clearKeys()
{
    mSetMap.clear();
}

int SetMap::delKey(const std::string& key)
{
    auto iter = mSetMap.find(key);
    if (iter == mSetMap.end()) {
        return 0;
    }
    
    SetType& sset = iter->second;
    sset.clear();
    
    mSetMap.erase(iter);
    return 1;
}

bool SetMap::keyExists(const std::string &key)
{
    return mSetMap.find(key) != mSetMap.end();
}
