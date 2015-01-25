#include "EventPublisher.h"

using namespace arag;
using namespace std;

void EventPublisher::subscribe(ISubscriber *pSubscr, int dbIndex)
{
    if (pSubscr) {
        mSubscribers[pSubscr] = dbIndex;
    }
}

void EventPublisher::fire(EventPublisher::Event event, const std::string& key, int dbIndex)
{
    for (auto subscr = mSubscribers.begin(); subscr != mSubscribers.end(); ++subscr) {
        if (subscr->second == -1 || subscr->second == dbIndex) {
            subscr->first->notify(event, key, dbIndex);
        }
    }
}

void EventPublisher::unsubscribe(ISubscriber *pSubscr)
{
    if (pSubscr) {
        mSubscribers.erase(pSubscr);
    }
}
