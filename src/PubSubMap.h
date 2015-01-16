#ifndef __arag__PubSubMap__
#define __arag__PubSubMap__

#include <unordered_map>
#include <string>
#include <list>
#include <vector>

namespace arag
{
    
class PubSubMap
{
public:
    typedef std::pair<std::list<int>, std::pair<bool, std::string>> PubSubElement;
    typedef std::unordered_map<std::string, PubSubElement> SubscribersMap;
    
public:
    
    void addSubscriber(const std::string& channel, int sid, bool pattern = false);
    
    std::list<PubSubElement> getSubscribers(const std::string& channel);
    
    void removeSubscriber(const std::string& channel, int sid);
    
    std::vector<std::string> unsubscribeFromAllChannels(int sid);
    
    void removeSubscriber(const std::vector<std::string>& patterns, int sid);
    
    int getSubscribersNum(int sid);
    
private:
    SubscribersMap mSubscrMap;    
};
    
};


#endif /* defined(__arag__PubSubMap__) */
