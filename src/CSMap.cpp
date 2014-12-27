#include <climits>
#include <ctime>
#include <stdlib.h>
#include "CSMap.h"
#include "RedisProtocol.h"
#include <iostream>


using namespace std;
using namespace cache_server;

CSMap::Item::Item()
{
}

CSMap::Item::Item(std::string val, int exp) : strVal(val), expSecs(exp)
{
    timestamp = (int)time(0);
}

CSMap::CSMap()
{
    counter = 0;
}

bool CSMap::isExpired(int timestamp, int expSecs)
{
    int currTimestamp = (int)time(0);
    //cout << "diff: " << timestamp + expSecs - currTimestamp << endl;
    return (expSecs != 0) && (timestamp + expSecs <= currTimestamp);
}

int CSMap::set(std::string key, std::string value, int expSecs)
{
    lock_guard<recursive_mutex> lock(mLock);
    counter++;
    map[key] = Item(value, expSecs);
    return (int)map[key].strVal.length();
}

string CSMap::getset(string key, string value, int expSecs)
{
    lock_guard<recursive_mutex> lock(mLock);
    string oldValue = map[key].strVal;
    
    set(key, value, expSecs);
    
    return oldValue;
}

string CSMap::get(std::string key)
{
    lock_guard<recursive_mutex> lock(mLock);
    auto iter = map.find(key);
    if (iter == map.end()) {
        throw invalid_argument("Wrong Key"); // WRONG_KEY
    }
    
    Item& item = iter->second;
    if (isExpired(item.timestamp, item.expSecs)) {
        throw invalid_argument("Key expired"); // WRONG_KEY
    }
    return iter->second.strVal;
}

int CSMap::append(std::string key, std::string value, int expSecs)
{
    lock_guard<recursive_mutex> lock(mLock);
    auto iter = map.find(key);
    if (iter == map.end()) {
        return set(key, "", expSecs);
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
    
    string str;
    if (start == end) {
        return "";
    }
    if (start < end) {
        return val.substr(start, end - start + 1);
    }
    
    return val.substr(start, len - start) + val.substr(0, end);
}

int CSMap::incr(string key)
{
    int intVal = 0;
    
    try {
        string val = get(key);
        
        size_t idx = 0;
        intVal = std::stoi(val, &idx);
        if (idx != val.length()) {
            throw;
        }

        intVal++;
    }
    catch (invalid_argument& e) {
    }

    set(key, to_string(intVal));
    return intVal;
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
        if (isExpired(item.timestamp, item.expSecs)) {
            map.erase(iter++);
            counter--;
        } else {
            ++iter;
        }
    }
}

