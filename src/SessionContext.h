#ifndef __arag__SessionContext__
#define __arag__SessionContext__

namespace arag
{

class SessionContext
{
public:
    
    SessionContext();
    
    void setDatabaseIndex(int index);

    void setAuthenticationStatus(bool authenticated);

    int getDatabaseIndex() const;
    
    bool isAuthenticated() const;
    
private:
    bool mIsAuthenticated;
    int mDatabaseIndex;
};
    
};

#endif /* defined(__arag__SessionContext__) */
