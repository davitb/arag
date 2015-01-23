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
    // Add the sid
    elem.first.push_back(sid);
    // Add channel type (pattern based or not)
    elem.second.first = pattern;
    // Add channel name
    elem.second.second = channel;
}

std::list<PubSubMap::PubSubElement> PubSubMap::getSubscribers(const std::string& channel)
{
    std::list<PubSubElement> subscrs;
    
    for (auto iter = mSubscrMap.begin(); iter != mSubscrMap.end(); ++iter) {
        // Check channel name against the pattern
        if (Utils::checkPubSubPattern(channel, iter->first)) {
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
    // For all channels:
    //   If sid is in subscriptions list:
    //     Add channel to the return list and remove it from subscriptions
    
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
        for (auto patt : patterns) {
            if (Utils::checkPubSubPattern(elem.first, patt)) {
                removeSubscriber(elem.first, sid);
            }
        }
    }
}

int PubSubMap::getSubscriptionsNum(int sid)
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
