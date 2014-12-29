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
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType cmdType;
};

class HGetCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 3
    };
};
  
class HExistsCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 3
    };
};

class HDelCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = INT_MAX
    };
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4
    };
    
    CmdType cmdType;
};
    
};

#endif /* defined(__arag__HashCmds__) */
