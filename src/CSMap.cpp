#include <climits>
#include <ctime>
#include <stdlib.h>
#include "CSMap.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <iostream>


using namespace std;
using namespace arag;

CSMap::Item::Item()
{
}

CSMap::Item::Item(std::string val, ExpirationType etype, int e) : strVal(val), exp(e), expType(etype)
{
    timestamp = (int)time(0);
}

CSMap::CSMap()
{
    counter = 0;
}

static std::string dbl2str(double d)
{
    size_t len = std::snprintf(0, 0, "%.10f", d);
    std::string s(len+1, 0);
    // technically non-portable, see below
    std::snprintf(&s[0], len+1, "%.10f", d);
    // remove nul terminator
    s.pop_back();
    // remove trailing zeros
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);
    // remove trailing point
    if(s.back() == '.') {
        s.pop_back();
    }
    return s;
}

static bool isExpired(int timestamp, CSMap::ExpirationType expType, int exp)
{
    // FIXME: Need to support miliseconds. time() returns in seconds

    int currTimestamp = (int)time(0);
    int finalTimestamp = timestamp + exp;
    return (exp != 0) && (finalTimestamp <= currTimestamp);
}

int CSMap::set(std::string key, std::string value,
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

string CSMap::getset(string key, string value)
{
    lock_guard<recursive_mutex> lock(mLock);
    Item& item = map[key];
    
    set(key, value, item.expType, item.exp);
    
    return item.strVal;
}

string CSMap::get(std::string key)
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

int CSMap::append(std::string key, std::string value)
{
    lock_guard<recursive_mutex> lock(mLock);
    auto iter = map.find(key);
    if (iter == map.end()) {
        return set(key, "");
    }
    iter->second.strVal += value;
    return (int)iter->second.strVal.length();
}

string CSMap::getRange(std::string key, int start, int end)
{
    string val = get(key);
    if (val == "") {
        return "";
    }
    
    int len = (int)val.length();
    
    if (abs(start) >= len) {
        start = len - 1;
    }
    
    if (abs(end) >= len) {
        end = len - 1;
    }
    
    if (start < 0) {
        start = len - abs(start);
    }
    
    if (end < 0) {
        end = len - abs(end);
    }
    
    if (start <= end) {
        return val.substr(start, end - start + 1);
    }
    
    return val.substr(start, len - start) + val.substr(0, end);
}

int CSMap::incrBy(string key, int by)
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

string CSMap::incrBy(string key, double by)
{
    double dval = 0;
    
    try {
        dval = Utils::convertToDouble(get(key)) + by;
    }
    catch (invalid_argument& e) {
    }
    
    string sVal = dbl2str(dval);
    
    set(key, sVal);
    return sVal;
}

vector<pair<string, int>> CSMap::mget(const vector<string>& keys)
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

int CSMap::getCounter() const
{
    return counter;
}

void CSMap::clearCounter()
{
    counter = 0;
}

void CSMap::cleanup()
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

