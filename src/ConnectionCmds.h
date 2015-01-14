#ifndef __arag__ConnectionCmds__
#define __arag__ConnectionCmds__

#include "Commands.h"

namespace arag
{

class PingCommand: public Command
{
public:
    
    enum CmdType
    {
        PING,
        ECHO
    };
    
    PingCommand(CmdType type) { mCmdType = type; }

    DEEP_CLONE(PingCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 1,
        MAX_ARG_NUM = 2
    };
    
    CmdType mCmdType;
};

class SelectCommand: public Command
{
public:
    
    DEEP_CLONE(SelectCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 2
    };
};

};

#endif /* defined(__arag__ConnectionCmds__) */
