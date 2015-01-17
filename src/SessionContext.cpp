#include "SessionContext.h"
#include "Utils.h"
#include "InMemoryData.h"

using namespace arag;
using namespace std;

SessionContext::SessionContext()
{
    mDatabaseIndex = 0;
    mIsAuthenticated = false;
    mSessionID = Utils::genRandom(0, INT_MAX);
    mTransactionInfo.transactionState = NO_TRANSACTION;
}

SessionContext::~SessionContext()
{
    if (mTransactionInfo.bIsSubscribed) {
        EventPublisher& pub = Database::instance().getEventPublisher();
        pub.unsubscribe(this);
    }
}

void SessionContext::setDatabaseIndex(int index)
{
    mDatabaseIndex = index;
}

void SessionContext::setAuthenticationStatus(bool authenticated)
{
    mIsAuthenticated = authenticated;
}

void SessionContext::setClientName(std::string name)
{
    mClientName = name;
}

string SessionContext::getClientName() const
{
    return mClientName;
}

void SessionContext::setConnectionDetails(std::string ip, int port)
{
    mIPAddress = ip + "/" + to_string(port);
}

string SessionContext::getIPAndPort() const
{
    return mIPAddress;
}

int SessionContext::getDatabaseIndex() const
{
    return mDatabaseIndex;
}

bool SessionContext::isAuthenticated() const
{
    return mIsAuthenticated;
}

bool SessionContext::isInTransaction() const
{
    return mTransactionInfo.transactionState == IN_TRANSACTION;
}

void SessionContext::setTransactionState(TransactionState trans)
{
    mTransactionInfo.transactionState = trans;
}

void SessionContext::addToTransactionQueue(std::shared_ptr<Command> cmd)
{
    mTransactionInfo.transactionQue.push_back(cmd);
}

const std::list<std::shared_ptr<Command>>& SessionContext::getTransactionQueue()
{
    return mTransactionInfo.transactionQue;
}

void SessionContext::clearTransactionQueue()
{
    mTransactionInfo.transactionQue.clear();
}

const std::unordered_map<std::string, bool>& SessionContext::getWatchedKeys()
{
    return mTransactionInfo.watchedKeys;
}

void SessionContext::finishTransaction()
{
    mTransactionInfo.transactionState = NO_TRANSACTION;
    mTransactionInfo.bTransactionAborted = false;
    clearTransactionQueue();
    clearWatchedKeys();
}

void SessionContext::markTransactionAborted()
{
    mTransactionInfo.bTransactionAborted = true;
}

void SessionContext::watchKey(const std::string &key)
{
    mTransactionInfo.watchedKeys[key] = true;
    
    if (!mTransactionInfo.bIsSubscribed) {
        EventPublisher& pub = Database::instance().getEventPublisher();
        pub.subscribe(this);
        mTransactionInfo.bIsSubscribed = true;
    }
}

void SessionContext::clearWatchedKeys()
{
    mTransactionInfo.bTransactionAborted = false;
    mTransactionInfo.watchedKeys.clear();
    if (mTransactionInfo.bIsSubscribed) {
        EventPublisher& pub = Database::instance().getEventPublisher();
        pub.unsubscribe(this);
        mTransactionInfo.bIsSubscribed = false;
    }
}

void SessionContext::notify(EventPublisher::Event event, const std::string& key, int db)
{
    auto item = mTransactionInfo.watchedKeys.find(key);
    if (item != mTransactionInfo.watchedKeys.end()) {
        if (!isInTransaction()) {
            markTransactionAborted();
        }
    }
}
