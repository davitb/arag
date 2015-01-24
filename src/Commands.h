#ifndef __CacheServer__Commands__
#define __CacheServer__Commands__

#include <string>
#include <vector>
#include <climits>
#include <memory>
#include "AragException.h"
#include "InMemoryData.h"
#include "SessionContext.h"
#include "Config.h"
#include "CommandResult.h"
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
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);\
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
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);\
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
    All arag commands are inherited from this class.
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
    
    // Command type
    enum Type
    {
        INTERNAL,
        EXTERNAL,
    };
    
    // Indicates some special characterisitcs of a command
    enum SpecialType
    {
        NORMAL,
        // These commands can bypass transaction state and be executed immediately
        BYPASS_TRANSACTION_STATE
    };
    
    /*
        Keeps a context information about a command
     */
    class Context
    {
    public:
        // Indicates which argument is a key. FIXME: this must be fixed.
        int mKeyArgIndex;
        IMapCommon::ContainerType mContainerType;

        Context()
        {
            mKeyArgIndex = -1;
            mContainerType = IMapCommon::NONE;
        }
        
        Context(int k, IMapCommon::ContainerType t)
        {
            mKeyArgIndex = k;
            mContainerType = t;
        }
    };
    
public:
    
    Command();
    
    virtual ~Command();
    
    virtual Command* clone() const = 0;
    
    // Execute the command and return its result
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx) = 0;

    // Execute the command and send the response either to the requesting client
    // or into pCmdResult
    static void executeEndToEnd(std::shared_ptr<Command> cmd,
                                int sessionID,
                                CommandResult* pCmdResult = nullptr);
    
    std::string getName() const;
    
    // Parses given request and returns a list of commands
    static void getCommand(const std::string& request,
                           std::vector<std::shared_ptr<Command>>& commands);

    // Converts prepared tokens into a single command
    static std::shared_ptr<Command> getCommand(const CommandResult::ResultArray& tokens);
    
    // Parses given request and returns a single command (this is used only internally)
    static std::shared_ptr<Command> getFirstCommand(const std::string& request);
    
    void setCommandContext(Context ctx);
    
    // Checks if the key that this command is going to operate on can be operated by this command
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
    
    // Sets the request tokens
    void setTokens(const std::vector<std::pair<std::string, int>>& tokens);
    
    friend class SelfTest;
    
protected:
    std::vector<std::pair<std::string, int>> mTokens;
    Context mCtx;
    Type mType;
    SpecialType mSpecialType;
    int mTimestamp;
    
private:
    
    // Processes internal commands
    Command::ResultType processInternalCommand();
};


/*
    Class for Internal Commands. 
    Internal commands which must be processed inside RP but don't come from outside
    are implemented in this class.
*/
class InternalCommand: public Command
{
public:
    
    InternalCommand(std::string name);
    
    DEEP_CLONE(InternalCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
};

    
};

#endif /* defined(__CacheServer__Commands__) */
