#include <climits>
#include <ctime>
#include <stdlib.h>
#include "StringMap.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <iostream>


using namespace std;
using namespace arag;

StringMap::Item::Item()
{
}

StringMap::Item::Item(std::string val, ExpirationType etype, int e) : strVal(val), exp(e), expType(etype)
{
    timestamp = (int)time(0);
}

StringMap::StringMap()
{
    counter = 0;
}

static bool isExpired(int timestamp, StringMap::ExpirationType expType, int exp)
{
    // FIXME: Need to support miliseconds. time() returns in seconds

    int currTimestamp = (int)time(0);
    int finalTimestamp = timestamp + exp;
    return (exp != 0) && (finalTimestamp <= currTimestamp);
}

int StringMap::set(std::string key, std::string value,
               ExpirationType expType, int exp, SetKeyPolicy policy)
{
    lock_guard<recursive_mutex> lock(mLock);
    
    if (policy != SetKeyPolicy::CREATE_IF_DOESNT_EXIST) {
        auto iter = map.find(key);
        if (iter == map.end()) {
            if (policy == SetKeyPolicy::ONLY_IF_ALREADY_EXISTS) {
                throw invalid_argument("Wrong Key"); // WRONG_KEY
            }
        }
        else {
            if (policy == SetKeyPolicy::ONLY_IF_DOESNT_ALREADY_EXISTS) {
                throw invalid_argument("Wrong Key"); // WRONG_KEY
            }
        }
    }
    
    counter++;
    map[key] = Item(value, expType, exp);
    return (int)map[key].strVal.length();
}

string StringMap::getset(string key, string value)
{
    lock_guard<recursive_mutex> lock(mLock);
    Item& item = map[key];
    string oldVal = item.strVal;
    
    set(key, value, item.expType, item.exp);
    
    return oldVal;
}

int StringMap::deleteKey(string key)
{
    try {
        get(key);
    }
    catch (...) {
        return 0;
    }
    
    map.erase(key);
    return 1;
}

string StringMap::get(std::string key)
{
    lock_guard<recursive_mutex> lock(mLock);
    auto iter = map.find(key);
    if (iter == map.end()) {
        throw invalid_argument("Wrong Key"); // WRONG_KEY
    }
    
    Item& item = iter->second;
    if (isExpired(item.timestamp, item.expType, item.exp)) {
        throw invalid_argument("Key expired"); // WRONG_KEY
    }
    return iter->second.strVal;
}

int StringMap::append(std::string key, std::string value)
{
    lock_guard<recursive_mutex> lock(mLock);
    auto iter = map.find(key);
    if (iter == map.end()) {
        return set(key, "");
    }
    iter->second.strVal += value;
    return (int)iter->second.strVal.length();
}

string StringMap::getRange(std::string key, int start, int end)
{
    string val = get(key);
    if (val == "") {
        return "";
    }
    
    int len = (int)val.length();
    
    Utils::normalizeIndexes(start, end, len);
    
    if (start >= len || start > end) {
        return "";
    }
    
    return val.substr(start, end - start + 1);
}

int StringMap::incrBy(string key, int by)
{
    int intVal = 0;
    
    try {
        intVal = Utils::convertToInt(get(key)) + by;
    }
    catch (invalid_argument& e) {
    }

    set(key, to_string(intVal));
    return intVal;
}

string StringMap::incrBy(string key, double by)
{
    double dval = 0;
    
    try {
        string val = get(key);
        dval = Utils::convertToDouble(val) + by;
    }
    catch (invalid_argument& e) {
    }
    
    string sVal = Utils::dbl2str(dval);
    
    set(key, sVal);
    return sVal;
}

vector<pair<string, int>> StringMap::mget(const vector<string>& keys)
{
    vector<pair<string, int>> vals;
    for (string key: keys) {
        try {
            vals.push_back(make_pair(get(key), (int)RedisProtocol::DataType::BULK_STRING));
        }
        catch (std::exception& e) {
            vals.push_back(make_pair("", (int)RedisProtocol::DataType::NILL));
        }
    }
    
    return vals;
}

vector<pair<string, int>> StringMap::getAll(int getAllType)
{
    vector<pair<string, int>> vals;
    for (auto elem = map.begin(); elem != map.end(); ++elem) {
        if (getAllType == KEYS_AND_VALUES || getAllType == KEYS) {
            vals.push_back(make_pair(elem->first, (int)RedisProtocol::DataType::BULK_STRING));
        }
        if (getAllType == KEYS_AND_VALUES || getAllType == VALUES) {
            vals.push_back(make_pair(elem->second.strVal, (int)RedisProtocol::DataType::BULK_STRING));
        }
    }

    return vals;
}

int StringMap::size()
{
    return (int)map.size();
}

int StringMap::getCounter() const
{
    return counter;
}

void StringMap::cleanup()
{
    lock_guard<recursive_mutex> lock(mLock);

    std::unordered_map<std::string, Item>::iterator iter = map.begin();
    for(; iter != map.end(); ) {
        Item& item = iter->second;
        if (isExpired(item.timestamp, item.expType, item.exp)) {
            map.erase(iter++);
            counter--;
        } else {
            ++iter;
        }
    }
}

void StringMap::clearKeys()
{
    lock_guard<recursive_mutex> lock(mLock);
    map.clear();
}
