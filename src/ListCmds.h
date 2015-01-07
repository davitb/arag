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
