#ifndef __arag__PubSubCmds__
#define __arag__PubSubCmds__

#include "Commands.h"

namespace arag
{
    
//----------------------------------------------------------------
    
class SubscribeCommand: public Command
{
public:
    
    DEEP_CLONE(SubscribeCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = INT_MAX
    };
};

//----------------------------------------------------------------

class PSubscribeCommand: public Command
{
public:
    
    DEEP_CLONE(PSubscribeCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = INT_MAX
    };
};
    
//----------------------------------------------------------------

class PublishCommand: public Command
{
public:
    
    DEEP_CLONE(PublishCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 3
    };
};

//----------------------------------------------------------------

class UnsubscribeCommand: public Command
{
public:
    
    DEEP_CLONE(UnsubscribeCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 1,
        MAX_ARG_NUM = INT_MAX
    };
};

//----------------------------------------------------------------

class PUnsubscribeCommand: public Command
{
public:
    
    DEEP_CLONE(PUnsubscribeCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 1,
        MAX_ARG_NUM = INT_MAX
    };
};
    
};


#endif /* defined(__arag__PubSubCmds__) */
