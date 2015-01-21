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

int ListMap::len(const std::string &key)
{
    if (!keyExists(key)) {
        return 0;
    }

    return (int)mListMap[key].size();
}

string ListMap::val(const std::string &key, int pos)
{
    if (!keyExists(key)) {
        throw invalid_argument("Wrong key");
    }
    
    list<string>& l = mListMap[key];
    
    if (abs(pos) >= l.size()) {
        throw invalid_argument("Out of range");
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
        throw invalid_argument("Wrong key");
    }
    
    list<string>& l = mListMap[key];
    
    if (abs(pos) >= l.size()) {
        throw invalid_argument("Out of range");
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
        throw invalid_argument("Wrong key");
    }
    
    list<string>& l = mListMap[key];
    string val;

    if (l.size() == 0) {
        throw invalid_argument("List is empty");
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
        throw invalid_argument("Wrong key");
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
        throw invalid_argument("Wrong key");
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
        throw invalid_argument("Wrong key");
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
        throw invalid_argument("Wrong key");
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


void ListMap::clearKeys()
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
