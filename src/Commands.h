#ifndef __CacheServer__Commands__
#define __CacheServer__Commands__

#include <string>
#include <vector>
#include <climits>
#include <memory>
#include "InMemoryData.h"
#include "SessionContext.h"
#include "Config.h"
#include "SelfTest.h"

namespace arag
{

namespace command_const
{
    static const std::string CMD_INTERNAL_STOP = "internal::stop";
    static const std::string CMD_EXTERNAL_EXIT = "exit";
    static const std::string CMD_INTERNAL_CLEANUP = "internal::cleanup";
    static const std::string CMD_INTERNAL_PREFIX = "internal::";
};

/*
    Base class for all commands.
 */
class Command
{
public:
    
    enum Type
    {
        NORMAL,
        TRANSACTION_START,
        TRANSACTION_END,
        WAITING_FOR_EVENT
    };
    
    class Context
    {
    public:
        int mKeyArgIndex;
        InMemoryData::ContainerType mContainerType;

        Context()
        {
            mKeyArgIndex = -1;
            mContainerType = InMemoryData::ContainerType::NONE;
        }
        
        Context(int k, InMemoryData::ContainerType t)
        {
            mKeyArgIndex = k;
            mContainerType = t;
        }
    };
    
public:
    
    Command();
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx) = 0;
    
    std::string getCommandName() const;
    
    static Command& getCommand(const std::string& cmdline);
    
    void setCommandContext(Context ctx);
    
    bool isKeyTypeValid(InMemoryData& db);
    
    Type getType() const
    {
        return mType;
    }
    
    void setType(Type t)
    {
        mType = t;
    }

protected:
    
    void setTokens(const std::vector<std::pair<std::string, int>>& tokens);
    
    void extractExpirationNum(const std::vector<std::pair<std::string, int>>& tokens,
                                       int minArgsNum,
                                       int maxArgsNum,
                                       StringMap::ExpirationType* pExpType,
                                       int* pExp);
    
    
    friend class SelfTest;
    
protected:
    std::vector<std::pair<std::string, int>> mTokens;
    Context mCtx;
    Type mType;
};

class InternalCommand: public Command
{
public:
    
    InternalCommand(std::string name);
    
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx)
    {
        throw std::logic_error("This function should not be called");
    }
};

    
};

#endif /* defined(__CacheServer__Commands__) */
