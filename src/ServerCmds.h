#ifndef __arag__ServerCmds__
#define __arag__ServerCmds__

#include "Commands.h"

namespace arag
{

class FlushCommand: public Command
{
public:
    
    enum CmdType
    {
        FLUSHDB,
        FLUSHALL
    };
    
    FlushCommand(CmdType type) { mCmdType = type; }
    
    DEEP_CLONE(FlushCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 1,
        MAX_ARG_NUM = 1
    };
    
    CmdType mCmdType;
};

class SingleArgumentCommand: public Command
{
public:
    
    enum CmdType
    {
        DBSIZE,
        LASTSAVE,
        TIME
    };
    
    SingleArgumentCommand(CmdType type) { mCmdType = type; }
    
    DEEP_CLONE(SingleArgumentCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 1,
        MAX_ARG_NUM = 1
    };
    
    CmdType mCmdType;
};
    
    
    COMMAND_CLASS(InfoCommand, 1, 2);
    
    COMMAND_CLASS(ClientCommand, 2, 3);
    
    COMMAND_CLASS(ConfigCommand, 2, 4);
    
    
};

#endif /* defined(__arag__ServerCmds__) */
