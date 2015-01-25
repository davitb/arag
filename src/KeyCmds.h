#ifndef __arag__KeyCmds__
#define __arag__KeyCmds__

#include "Commands.h"

namespace arag
{

COMMAND_CLASS(DelCommand, 2, INT_MAX);

COMMAND_CLASS(ExistsCommand, 2, 2);

COMMAND_CLASS(TypeCommand, 2, 2);
    
COMMAND_CLASS(KeysCommand, 2, 2);
    
COMMAND_CLASS(PersistCommand, 2, 2);

COMMAND_CLASS(RandomKeyCommand, 1, 1);
    
class TTLCommand: public Command
{
public:
    
    enum CmdType
    {
        TTL,
        PTTL
    };
    
    TTLCommand(CmdType type) { mCmdType = type; }
    
    DEEP_CLONE(TTLCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 2
    };
    
    CmdType mCmdType;
};
    
};


#endif /* defined(__arag__KeyCmds__) */
