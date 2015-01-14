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

class LRangeCommand: public Command
{
public:
    
    DEEP_CLONE(LRangeCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4
    };
};

class LSetCommand: public Command
{
public:
    
    DEEP_CLONE(LSetCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4
    };
};

class LTrimCommand: public Command
{
public:
    
    DEEP_CLONE(LTrimCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4
    };
};

class LInsertCommand: public Command
{
public:
    
    DEEP_CLONE(LInsertCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 5,
        MAX_ARG_NUM = 5
    };
};
    
};

#endif /* defined(__arag__ListCmds__) */
