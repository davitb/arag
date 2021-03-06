#include <climits>
#include <ctime>
#include <iterator>
#include "ListMap.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <iostream>


using namespace std;
using namespace arag;

int ListMap::push(const string& key, const string& val, Position direction)
{
    list<string>& l = mListMap[key];
    
    switch (direction) {
        case FRONT:
            l.push_front(val);
            break;
            
        case BACK:
            l.push_back(val);
            break;
    }
    
    return (int)mListMap[key].size();
}

int ListMap::size()
{
    return (int)mListMap.size();
}

int ListMap::size(const std::string &key)
{
    if (!keyExists(key)) {
        return 0;
    }

    return (int)mListMap[key].size();
}

string ListMap::val(const std::string &key, int pos)
{
    if (!keyExists(key)) {
        throw EInvalidKey();
    }
    
    list<string>& l = mListMap[key];
    
    if (abs(pos) >= l.size()) {
        throw EInvalidArgument();
    }
    
    if (pos < 0) {
        pos = (int)l.size() + pos;
    }
    
    auto iter = l.begin();
    std::advance(iter, pos);
    
    return *iter;
}

void ListMap::setVal(const std::string &key, int pos, const std::string &val)
{
    if (!keyExists(key)) {
        throw EInvalidKey();
    }
    
    list<string>& l = mListMap[key];
    
    if (abs(pos) >= l.size()) {
        throw EInvalidArgument();
    }
    
    if (pos < 0) {
        pos = (int)l.size() + pos;
    }
    
    auto iter = l.begin();
    std::advance(iter, pos);
    
    *iter = val;
}

string ListMap::pop(const std::string &key, Position pos)
{
    if (!keyExists(key)) {
        throw EInvalidKey();
    }
    
    list<string>& l = mListMap[key];
    string val;

    if (l.size() == 0) {
        throw EInvalidArgument();
    }
    
    switch (pos)
    {
        case FRONT:
        {
            val = l.front();
            l.pop_front();
            break;
        }
            
        case BACK:
        {
            val = l.back();
            l.pop_back();
            break;
        }
    }

    if (l.size() == 0) {
        mListMap.erase(key);
    }
    
    return val;
}

static int removeElement(list<string>& l,
                         list<string>::iterator begin,
                         list<string>::iterator end,
                         const string& val,
                         int count,
                         bool check,
                         bool forward)
{
    int numRemoved = 0;
    
    auto i = begin;
    while (i != end) {
        if (check && count == 0) {
            return numRemoved;
        }
        if ((*i) == val) {
            l.erase(i);
            count--;
            numRemoved++;
        }
        if (forward) {
            i++;
        }
        else {
            i--;
        }
    }
    return numRemoved;
}

int ListMap::rem(const std::string &key, const std::string &val, int count)
{
    if (!keyExists(key)) {
        throw EInvalidKey();
    }
    
    list<string>& l = mListMap[key];
    
    if (l.size() == 0) {
        return 0;
    }

    if (count == 0) {
        return removeElement(l, l.begin(), l.end(), val, count, false, true);
    }
    else
    if (count > 0) {
        return removeElement(l, l.begin(), l.end(), val, count, true, true);
    }
    else {
        return removeElement(l, l.end(), l.begin(), val, abs(count), true, false);
    }
}

vector<pair<string, int>> ListMap::getRange(const std::string &key, int start, int end)
{
    if (!keyExists(key)) {
        throw EInvalidKey();
    }
    
    list<string>& l = mListMap[key];
    
    vector<pair<string, int>> range;
    
    Utils::normalizeIndexes(start, end, (int)l.size());

    auto itStart = l.begin();
    std::advance(itStart, start);

    auto itEnd = l.begin();
    std::advance(itEnd, end + 1);
    
    if (start >= l.size() || start > end) {
        return range;
    }
    
    if (start == end) {
        range.push_back(make_pair(*itStart, RedisProtocol::BULK_STRING));
    }
    else
    if (start < end) {
        for_each(itStart, itEnd, [&range](const string& str) {
            range.push_back(make_pair(str, RedisProtocol::BULK_STRING));
        });
    }
    
    return range;
}

void ListMap::trim(const std::string &key, int start, int end)
{
    if (!keyExists(key)) {
        throw EInvalidKey();
    }
    
    list<string>& l = mListMap[key];
    
    Utils::normalizeIndexes(start, end, (int)l.size());
    
    if (start >= l.size() || start > end) {
        return;
    }

    list<string> newlist;
    
    auto itStart = l.begin();
    std::advance(itStart, start);

    while (start++ != end + 1) {
        newlist.push_back(*(itStart++));
    }

    mListMap[key] = newlist;
}

int ListMap::insert(const std::string &key,
                    const std::string &pos,
                    const std::string &pivot,
                    const std::string &val)
{
    if (!keyExists(key)) {
        throw EInvalidKey();
    }
    
    list<string>& l = mListMap[key];

    auto iter = std::find(l.begin(), l.end(), pivot);
    if (iter == l.end()) {
        return -1;
    }
    
    if (pos == "BEFORE") {
        l.insert(iter, val);
    }
    else {
        l.insert(++iter, val);
    }
    
    return (int)l.size();
}


void ListMap::flush()
{
    mListMap.clear();
}


int ListMap::delKey(const std::string& key)
{
    auto iter = mListMap.find(key);
    if (iter == mListMap.end()) {
        return 0;
    }
    
    ListType& l = iter->second;
    l.clear();
    
    mListMap.erase(iter);
    return 1;
}

bool ListMap::keyExists(const std::string &key)
{
    return mListMap.find(key) != mListMap.end();
}

IMapCommon::ContainerType ListMap::getContainerType()
{
    return IMapCommon::LIST;
}

int ListMap::rename(const std::string &key, const std::string &newKey)
{
    mListMap[newKey] = ListType();
    mListMap[newKey].swap(mListMap[key]);
    mListMap.erase(key);
    
    return 1;
}

void ListMap::sort(const std::string& key,
                   std::string destKey,
                   bool asc,
                   bool alpha,
                   int offset,
                   int limit,
                   RedisArray& arr)
{
    if (!keyExists(key)) {
        throw EInvalidKey();
    }
    
    // Need to make a copy since we don't want to sort the actual list
    list<string> l = mListMap[key];

    if (offset < 0 || offset > l.size() || limit < 0) {
        throw EInvalidKey();
    }
        
    if (offset + limit > l.size()) {
        limit = (int)l.size() - offset;
    }
    
    if (limit == 0) {
        limit = (int)l.size();
    }

    function<bool(const string&, const string&)> fCompare = [alpha, asc] (const string& first,
                                                                          const string& second) {
        
        if (alpha == false) {
            double f = Utils::convertToDouble(first);
            double s = Utils::convertToDouble(second);
            
            return asc ? f < s : s < f;
        }
        
        return asc ? first.compare(second) < 0 : second.compare(first) < 0;
    };
    
    l.sort(fCompare);
    
    auto iter = l.begin();
    std::advance(iter, offset);
    
    while (limit-- != 0 && iter != l.end()) {
        arr.push_back(make_pair(*iter, RedisProtocol::BULK_STRING));
        iter++;
    }
}

