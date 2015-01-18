#ifndef __arag__SessionContext__
#define __arag__SessionContext__

#include "EventPublisher.h"
#include <string>
#include <list>
#include <unordered_map>

namespace arag
{

class ClientSession;
class Command;
    
class SessionContext : ISubscriber
{
public:
    
    enum Consts
    {
        FAKE_SESSION = -1
    };
    
    enum TransactionState
    {
        IN_TRANSACTION,
        NO_TRANSACTION
    };
        
    SessionContext();
    
    ~SessionContext();
    
    void setDatabaseIndex(int index);

    void setAuthenticationStatus(bool authenticated);
    
    void setClientName(std::string name);

    void setConnectionDetails(std::string ip, int port);
    
    std::string getIPAndPort() const;

    std::string getClientName() const;
    
    int getDatabaseIndex() const;
    
    bool isAuthenticated() const;
    
    ClientSession& getClientSession() const;
    
    void setSessionID(int sid)
    {
        mSessionID = sid;
    }
    
    int getSessionID() const
    {
        return mSessionID;
    }
    
    virtual void notify(EventPublisher::Event event, const std::string& key, int db);
    
    // Transaction related functions
    // --------------------------------------------------------
    
    bool isInTransaction() const;

    bool isTransactionAborted() const
    {
        return mTransactionInfo.bTransactionAborted;
    }
    
    void setTransactionState(TransactionState trans);
    
    void addToTransactionQueue(std::shared_ptr<Command> cmd);
    
    const std::list<std::shared_ptr<Command>>& getTransactionQueue();
    
    void clearTransactionQueue();

    void watchKey(const std::string& key);
    
    const std::list<std::string>& getWatchedKeys();
    
    void clearWatchedKeys();

    void finishTransaction();
    
    void markTransactionAborted();
    
    
    // BlockingList related functions
    // --------------------------------------------------------
    
    /*
        Sets the pending BL command.
     */
    void setPendingtBLCommand(std::shared_ptr<Command> cmd,
                      int timeout,
                      const std::list<std::string>& blKeys);

    /*
        This functon is called every time a new command is sent within this client context.
        If there was a pending BL command - it checks if it's timed out.
        If it's not timed out - the function just returns.
        If it's timed out - the pending command is forgotten since based on Redis documentation -
        Client has already sent the response.
     */
    void checkPendingBLCommand();
    
private:
    struct TransactionInfo
    {
        TransactionState transactionState;
        std::list<std::shared_ptr<Command>> transactionQue;
        bool bTransactionAborted;
        std::list<std::string> watchedKeys;
    };
    
    struct PendingBLCommand
    {
        std::shared_ptr<Command> cmd;
        int timeout;
        int timestamp;
        std::list<std::string> watchedKeys;
    };
    
    bool mIsAuthenticated;
    int mDatabaseIndex;
    std::string mClientName;
    int mSessionID;
    std::string mIPAddress;
    TransactionInfo mTransactionInfo;
    PendingBLCommand mPendingBLCmd;
    bool mIsSubscribed;
    
private:
    
    void subscribeToNotifications();
    
    void unsubscribeFromNotifications(bool force = false);
    
    void clearPendingBLCommand();
};

};

#endif /* defined(__arag__SessionContext__) */
