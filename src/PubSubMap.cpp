#include "PubSubMap.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <iostream>

using namespace std;
using namespace arag;
using namespace hll;

void PubSubMap::addSubscriber(const std::string& channel, int sid, bool pattern)
{
    PubSubElement& elem = mSubscrMap[channel];
    elem.first.push_back(sid);
    elem.second.first = pattern;
    elem.second.second = channel;
}

std::list<PubSubMap::PubSubElement> PubSubMap::getSubscribers(const std::string& channel)
{
    std::list<PubSubElement> subscrs;
    
    for (auto iter = mSubscrMap.begin(); iter != mSubscrMap.end(); ++iter) {
        string ch = iter->first;
        if (Utils::checkPattern(channel, iter->first)) {
            subscrs.push_back(iter->second);
        }
    }
    
    return subscrs;
}

void PubSubMap::removeSubscriber(const std::string& channel, int sid)
{
    auto elem = mSubscrMap.find(channel);
    if (elem == mSubscrMap.end()) {
        return;
    }
    
    elem->second.first.remove(sid);
}

vector<string> PubSubMap::unsubscribeFromAllChannels(int sid)
{
    vector<string> chnls;
    for (auto iter = mSubscrMap.begin(); iter != mSubscrMap.end(); ++iter) {
        const std::list<int>& l = iter->second.first;
        auto elem = std::find(l.begin(), l.end(), sid);
        if (elem == l.end()) {
            continue;
        }
        chnls.push_back(iter->first);
        iter->second.first.remove(sid);
    }
    
    return chnls;
}

void PubSubMap::removeSubscriber(const std::vector<std::string>& patterns, int sid)
{
    for (auto elem : mSubscrMap) {
        std::string channel = elem.first;
        for (auto patt : patterns) {
            if (Utils::checkPattern(channel, patt)) {
                removeSubscriber(channel, sid);
            }
        }
    }
}

int PubSubMap::getSubscribersNum(int sid)
{
    int num = 0;
    for (auto iter = mSubscrMap.begin(); iter != mSubscrMap.end(); ++iter) {
        const std::list<int>& l = iter->second.first;
        auto elem = std::find(l.begin(), l.end(), sid);
        if (elem != l.end()) {
            num++;
        }
    }
    
    return num;
}