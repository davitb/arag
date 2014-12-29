#ifndef __arag__ListCmds__
#define __arag__ListCmds__

#include "Commands.h"

namespace arag
{

class LPushCommand: public Command
{
public:
    
    enum CmdType
    {
        RPUSH,
        RPUSHX,
        LPUSH,
        LPUSHX
    };
    
    LPushCommand(CmdType type) { mCmdType = type; }
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType mCmdType;
};

};

#endif /* defined(__arag__ListCmds__) */
