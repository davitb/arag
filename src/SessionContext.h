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
    
/*
    Every client session has a context where session specific information is stored.
*/
class SessionContext : ISubscriber
{
public:
    
    enum Consts
    {
        // Fake session is used with internal commands only
        FAKE_SESSION = -1
    };
    
    enum TransactionState
    {
        // When Multi command is issued the session enters into IN_TRANSACTION state
        IN_TRANSACTION,
        NO_TRANSACTION
    };
    
    enum ScanCmdType
    {
        SCAN = 0, SSCAN = 1, HSCAN = 2, ZSCAN = 3
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
    
    // Return the session current context is associated with
    ClientSession& getClientSession() const;
    
    void setSessionID(int sid)
    {
        mSessionID = sid;
    }
    
    int getSessionID() const
    {
        return mSessionID;
    }
    
    // Implements ISubscriber interface to listen for events generated by arag commands
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
    
    // Sets tiemstamp when last time SCAN command has been called with cursor=0
    void setScanCommandStartTime(ScanCmdType type);
    
    // Returns tiemstamp when last time SCAN command has been called with cursor=0
    int getScanCommandStartTime(ScanCmdType type);
    
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
    // True when context is sibscribed to notifications
    bool mIsSubscribed;
    // SCAN family has 4 commands
    int _scanCmdTimes[4];
    
private:
    
    void subscribeToNotifications();
    
    void unsubscribeFromNotifications(bool force = false);
    
    void clearPendingBLCommand();
};

};

#endif /* defined(__arag__SessionContext__) */
