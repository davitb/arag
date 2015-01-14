#ifndef __arag__KeyCmds__
#define __arag__KeyCmds__

#include "Commands.h"

namespace arag
{
    
//----------------------------------------------------------------

class DelCommand: public Command
{
public:
    
    DEEP_CLONE(DelCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = INT_MAX
    };
};

//----------------------------------------------------------------

class ExistsCommand: public Command
{
public:
    
    DEEP_CLONE(ExistsCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 2
    };
};

};


#endif /* defined(__arag__KeyCmds__) */
