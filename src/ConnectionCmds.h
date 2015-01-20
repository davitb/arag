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
        ECHO_CMD
    };
    
    PingCommand(CmdType type) { mCmdType = type; }

    DEEP_CLONE(PingCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 1,
        MAX_ARG_NUM = 2
    };
    
    CmdType mCmdType;
};

    COMMAND_CLASS(SelectCommand, 2, 2);
    
};

#endif /* defined(__arag__ConnectionCmds__) */
