#include "SessionContext.h"
#include "Utils.h"
#include "InMemoryData.h"
#include "AragServer.h"
#include "Database.h"

using namespace arag;
using namespace std;

SessionContext::SessionContext()
{
    mDatabaseIndex = 0;
    mIsAuthenticated = false;
    // Generate a random session ID
    mSessionID = Utils::genRandom(0, INT_MAX);
    mTransactionInfo.transactionState = NO_TRANSACTION;
    mClientName = "";
}

SessionContext::~SessionContext()
{
    unsubscribeFromNotifications(true);
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

const std::list<std::string>& SessionContext::getWatchedKeys()
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
    mTransactionInfo.watchedKeys.push_back(key);
    
    subscribeToNotifications();
}

void SessionContext::clearWatchedKeys()
{
    mTransactionInfo.bTransactionAborted = false;
    mTransactionInfo.watchedKeys.clear();
    unsubscribeFromNotifications();
}

void SessionContext::subscribeToNotifications()
{
    if (!mIsSubscribed) {
        EventPublisher& pub = Database::instance().getEventPublisher();
        pub.subscribe(this);
        mIsSubscribed = true;
    }
}

void SessionContext::unsubscribeFromNotifications(bool force)
{
    bool condition = (mIsSubscribed) &&
                    ((force) || (mTransactionInfo.watchedKeys.empty() && mPendingBLCmd.watchedKeys.empty()));
    
    if (condition) {
        EventPublisher& pub = Database::instance().getEventPublisher();
        pub.unsubscribe(this);
        mIsSubscribed = false;
    }
}

void SessionContext::notify(EventPublisher::Event event, const std::string& key, int db)
{
    // This function is called on any event. Check to see if this provided key is in
    // watchKeys map.
    auto item = std::find(mTransactionInfo.watchedKeys.begin(),
                          mTransactionInfo.watchedKeys.end(),
                          key);
    if (item != mTransactionInfo.watchedKeys.end()) {
        if (!isInTransaction()) {
            // We need to mark the transaction as "aborted" since a watchKey has been changed.
            markTransactionAborted();
        }
    }

    // This function is called on any event. Check to see if this provided key is in
    // watchKeys map.
    item = std::find(mPendingBLCmd.watchedKeys.begin(),
                     mPendingBLCmd.watchedKeys.end(),
                     key);
    if (item != mPendingBLCmd.watchedKeys.end()) {
        std::shared_ptr<Command> pendingCmd = mPendingBLCmd.cmd;
        RequestProcessor::Request req(pendingCmd, mSessionID);
        
        // We have been notified that a watchKey has elements now.
        // Enqueue the BL request to RP and try to execute it again
        clearPendingBLCommand();
        Arag::instance().getRequestProcessor().enqueueRequest(req);
    }
}

void SessionContext::setPendingtBLCommand(std::shared_ptr<Command> cmd,
                                          int timeout,
                                          const std::list<std::string>& blKeys)
{
    clearPendingBLCommand();
    
    mPendingBLCmd.cmd = cmd;
    mPendingBLCmd.timeout = timeout;
    mPendingBLCmd.watchedKeys = blKeys;
    mPendingBLCmd.timestamp = (int)time(0);
    
    subscribeToNotifications();
}

void SessionContext::checkPendingBLCommand()
{
    if (mPendingBLCmd.watchedKeys.empty()) {
        return;
    }
    
    int currTimestamp = (int)time(0);
    int finalTimestamp = mPendingBLCmd.timestamp + mPendingBLCmd.timeout;
    
    if (mPendingBLCmd.timeout != 0 && finalTimestamp <= currTimestamp) {
        clearPendingBLCommand();
    }
}

void SessionContext::clearPendingBLCommand()
{
    mPendingBLCmd.cmd.reset();
    mPendingBLCmd.timeout = 0;
    mPendingBLCmd.watchedKeys.clear();
    mPendingBLCmd.timestamp = 0;
    
    unsubscribeFromNotifications();
}

