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

#define DEEP_CLONE(class) \
virtual class* clone() const \
{\
    return new class(*this);\
}\

#define COMMAND_CLASS(className, minArg, maxArg) \
class className: public Command \
{\
public:\
    \
    DEEP_CLONE(className)\
    \
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);\
    \
private:\
    enum Consts\
    {\
        MIN_ARG_NUM = minArg,\
        MAX_ARG_NUM = maxArg\
    };\
};\

#define COMMAND_CLASS_WITH_CONSTRUCTOR(className, minArg, maxArg) \
class className: public Command \
{\
    public:\
    className();\
    \
    DEEP_CLONE(className)\
    \
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);\
    \
    private:\
    enum Consts\
    {\
        MIN_ARG_NUM = minArg,\
        MAX_ARG_NUM = maxArg\
    };\
};\


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

    // Internal operations will return intent.
    enum ResultType
    {
        // Skip further execution
        SKIP,
        // Stop the thread
        STOP,
        // Continue with execution
        CONTINUE
    };
    
    enum Type
    {
        INTERNAL,
        EXTERNAL,
    };
    
    enum SpecialType
    {
        NORMAL,
        BYPASS_TRANSACTION_STATE
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
    
    virtual Command* clone() const = 0;
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx) = 0;
    
    static void executeEndToEnd(std::shared_ptr<Command> cmd,
                                int sessionID,
                                std::vector<std::string>* pResponeList = nullptr);
    
    std::string getCommandName() const;
    
    static void getCommand(const std::string& cmdline,
                           std::vector<std::shared_ptr<Command>>& commands);

    static std::shared_ptr<Command> getCommand(const std::string& cmdline);
    
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

    SpecialType getSpecialType() const
    {
        return mSpecialType;
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
    SpecialType mSpecialType;
    
private:
    
    Command::ResultType processInternalCommand();
};

class InternalCommand: public Command
{
public:
    
    InternalCommand(std::string name);
    
    DEEP_CLONE(InternalCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx)
    {
        throw std::logic_error("This function should not be called");
    }
};

    
};

#endif /* defined(__CacheServer__Commands__) */
