#ifndef __arag__SessionContext__
#define __arag__SessionContext__

#include <string>

namespace arag
{

class ClientSession;
    
class SessionContext
{
public:
    
    enum Consts
    {
        FAKE_SESSION = -1
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
    
private:
    bool mIsAuthenticated;
    int mDatabaseIndex;
    std::string mClientName;
    int mSessionID;
    std::string mIPAddress;
};

};

#endif /* defined(__arag__SessionContext__) */
