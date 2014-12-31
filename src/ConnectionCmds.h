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
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 1,
        MAX_ARG_NUM = 2
    };
    
    CmdType mCmdType;
};

};

#endif /* defined(__arag__ConnectionCmds__) */
