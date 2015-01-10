#ifndef __arag__SessionContext__
#define __arag__SessionContext__

#include <string>

namespace arag
{

class SessionContext
{
public:
    
    SessionContext();
    
    void setDatabaseIndex(int index);

    void setAuthenticationStatus(bool authenticated);
    
    void setClientName(std::string name);

    void setConnectionDetails(std::string ip, int port);
    
    std::string getIPAndPort() const;

    std::string getClientName() const;
    
    int getDatabaseIndex() const;
    
    bool isAuthenticated() const;
    
private:
    bool mIsAuthenticated;
    int mDatabaseIndex;
    std::string mClientName;
    int mClientID;
    std::string mIPAddress;
};
    
};

#endif /* defined(__arag__SessionContext__) */
