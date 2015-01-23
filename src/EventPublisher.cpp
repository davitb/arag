#include "EventPublisher.h"

using namespace arag;
using namespace std;

void EventPublisher::subscribe(ISubscriber *pSubscr)
{
    if (pSubscr) {
        mSubscribers[pSubscr] = true;
    }
}

void EventPublisher::fire(EventPublisher::Event event, const std::string& key, int db)
{
    for (auto subscr = mSubscribers.begin(); subscr != mSubscribers.end(); ++subscr) {
        subscr->first->notify(event, key, db);
    }
}

void EventPublisher::unsubscribe(ISubscriber *pSubscr)
{
    if (pSubscr) {
        mSubscribers.erase(pSubscr);
    }
}
