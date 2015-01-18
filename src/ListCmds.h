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
    
    DEEP_CLONE(LPushCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType mCmdType;
};

class LGetCommand: public Command
{
public:
    
    enum CmdType
    {
        LEN,
        INDEX
    };
    
    LGetCommand(CmdType type) { mCmdType = type; }
    
    DEEP_CLONE(LGetCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 3
    };
    
    CmdType mCmdType;
};

class LRemCommand: public Command
{
public:
    
    enum CmdType
    {
        REM,
        LPOP,
        RPOP,
        RPOPLPUSH
    };
    
    LRemCommand(CmdType type) { mCmdType = type; }
    
    DEEP_CLONE(LRemCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 4
    };
    
    CmdType mCmdType;
};

class BLCommand: public Command
{
public:
    
    enum CmdType
    {
        BLPOP,
        BRPOP
    };
    
    BLCommand(CmdType type) { mCmdType = type; }
    
    DEEP_CLONE(BLCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType mCmdType;
};

COMMAND_CLASS(BRPopLPushCommand, 4, 4);
    
COMMAND_CLASS(LRangeCommand, 4, 4);

COMMAND_CLASS(LSetCommand, 4, 4);

COMMAND_CLASS(LTrimCommand, 4, 4);

COMMAND_CLASS(LInsertCommand, 4, 4);

};

#endif /* defined(__arag__ListCmds__) */
