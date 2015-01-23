#ifndef __arag__PubSubMap__
#define __arag__PubSubMap__

#include <unordered_map>
#include <string>
#include <list>
#include <vector>

namespace arag
{
    
/*
    Implements the structures for handling publish/subscribe functionality.
    Clients subscribe to channels and then get updated when a publisher 
    sends a message to a channel.
 */
class PubSubMap
{
public:
    // PubSubElement holds information about subscribers for a channel. It indicates
    // if the channel is pattern based or not and also contains the name of channel itself.
    typedef std::pair<std::list<int>, std::pair<bool, std::string>> PubSubElement;
    
    typedef std::unordered_map<std::string, PubSubElement> SubscribersMap;
    
public:
    
    void addSubscriber(const std::string& channel, int sid, bool pattern = false);

    // Return subscribers for the given channel
    std::list<PubSubElement> getSubscribers(const std::string& channel);

    // Remove subcriber from the given channel
    void removeSubscriber(const std::string& channel, int sid);

    // Remove subcriber from all subscribed channels and return these channels
    std::vector<std::string> unsubscribeFromAllChannels(int sid);
    
    // Remove subcriber from channels that match any pattern from the given list
    void removeSubscriber(const std::vector<std::string>& patterns, int sid);

    // Return the number of subscriptions for this subscriber
    int getSubscriptionsNum(int sid);
    
private:
    SubscribersMap mSubscrMap;    
};
    
};


#endif /* defined(__arag__PubSubMap__) */
