#ifndef __arag__HashCmds__
#define __arag__HashCmds__

#include "InMemoryData.h"
#include "Commands.h"

namespace arag
{
    
class HSetCommand: public Command
{
public:
    
    enum CmdType
    {
        SET,
        SETNX,
        MSET
    };
    
    HSetCommand(CmdType type) { cmdType = type; }
    
    DEEP_CLONE(HSetCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType cmdType;
};

class HGetAllCommand: public Command
{
public:
    
    enum CmdType
    {
        GETALL,
        KEYS,
        VALS,
        MGET,
        LEN
    };
    
    HGetAllCommand(CmdType type) { cmdType = type; };
    
    DEEP_CLONE(HGetAllCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType cmdType;
};

class HIncrByCommand: public Command
{
public:
    
    enum CmdType
    {
        INCRBY,
        INCRBYFLOAT
    };
    
    HIncrByCommand(CmdType type) { cmdType = type; };
    
    DEEP_CLONE(HIncrByCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4
    };
    
    CmdType cmdType;
};

COMMAND_CLASS(HGetCommand, 3, 3);
COMMAND_CLASS(HExistsCommand, 3, 3);
COMMAND_CLASS(HDelCommand, 3, INT_MAX);
COMMAND_CLASS(HScanCommand, 3, 7);

};

#endif /* defined(__arag__HashCmds__) */
