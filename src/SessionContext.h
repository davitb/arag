#ifndef __arag__SessionContext__
#define __arag__SessionContext__

#include <string>
#include <list>

namespace arag
{

class ClientSession;
class Command;
    
class SessionContext
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
    
    bool isInTransaction() const
    {
        return mTransactionInfo.transactionState == IN_TRANSACTION;
    }
    
    void setTransactionState(TransactionState trans)
    {
        mTransactionInfo.transactionState = trans;
    }
    
    void addToTransactionQueue(std::shared_ptr<Command> cmd)
    {
        mTransactionInfo.transactionQue.push_back(cmd);
    }
    
    const std::list<std::shared_ptr<Command>>& getTransactionQueue()
    {
        return mTransactionInfo.transactionQue;
    }
    
    void clearTransactionQueue()
    {
        mTransactionInfo.transactionQue.clear();
    }

    void watchKey(const std::string& key)
    {
        mTransactionInfo.watchedKeys.push_back(key);
    }
    
    const std::list<std::string>& getWatchedKeys()
    {
        return mTransactionInfo.watchedKeys;
    }
    
    void clearWatchedKeys()
    {
        mTransactionInfo.watchedKeys.clear();
    }
    
    void abortTransaction()
    {
        mTransactionInfo.bTransactionAborted = true;
        mTransactionInfo.transactionState = NO_TRANSACTION;
        clearTransactionQueue();
        clearWatchedKeys();
    }
    
private:
    struct TransactionInfo
    {
        TransactionState transactionState;
        std::list<std::shared_ptr<Command>> transactionQue;
        bool bTransactionAborted;
        std::list<std::string> watchedKeys;
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
