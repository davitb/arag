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
    
    const std::unordered_map<std::string, bool>& getWatchedKeys();
    
    void clearWatchedKeys();

    void finishTransaction();
    
    void markTransactionAborted();
    
    virtual void notify(EventPublisher::Event event, const std::string& key, int db);    
    
private:
    struct TransactionInfo
    {
        TransactionState transactionState;
        std::list<std::shared_ptr<Command>> transactionQue;
        bool bTransactionAborted;
        std::unordered_map<std::string, bool> watchedKeys;
        bool bIsSubscribed;
    };
    
    bool mIsAuthenticated;
    int mDatabaseIndex;
    std::string mClientName;
    int mSessionID;
    std::string mIPAddress;
    TransactionInfo mTransactionInfo;
};

};

#endif /* defined(__arag__SessionContext__) */
