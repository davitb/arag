#include "SessionContext.h"

using namespace arag;

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

int SessionContext::getDatabaseIndex() const
{
    return mDatabaseIndex;
}

bool SessionContext::isAuthenticated() const
{
    return mIsAuthenticated;
}
