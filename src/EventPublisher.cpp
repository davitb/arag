#include "EventPublisher.h"

using namespace arag;
using namespace std;

void EventPublisher::subscribe(ISubscriber *pSubscr)
{
    if (pSubscr) {
        mSubscribers.push_back(pSubscr);
    }
}

void EventPublisher::fire(EventPublisher::Event event, const std::string& key, int db)
{
    for (auto pSubscr : mSubscribers) {
        if (pSubscr) {
            pSubscr->notify(event, key, db);
        }
    }
}

void EventPublisher::unsubscribe(ISubscriber *pSubscr)
{
    if (pSubscr) {
        mSubscribers.remove(pSubscr);
    }
}
