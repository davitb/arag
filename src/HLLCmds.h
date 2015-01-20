#ifndef __arag__HLLCmds__
#define __arag__HLLCmds__

#include "Commands.h"

namespace arag
{
    
//----------------------------------------------------------------

class PFAddCommand: public Command
{
public:
    
    DEEP_CLONE(PFAddCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = INT_MAX
    };
};

class PFCountCommand: public Command
{
public:
    
    DEEP_CLONE(PFCountCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = INT_MAX
    };
};

class PFMergeCommand: public Command
{
public:
    
    DEEP_CLONE(PFMergeCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = INT_MAX
    };
};
    
};


#endif /* defined(__arag__HLLCmds__) */
