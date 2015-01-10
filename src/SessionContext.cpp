#include "SessionContext.h"

using namespace arag;
using namespace std;

SessionContext::SessionContext()
{
    mDatabaseIndex = 0;
    mIsAuthenticated = false;
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
