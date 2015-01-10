#ifndef __arag__ServerCmds__
#define __arag__ServerCmds__

#include "Commands.h"

namespace arag
{

class InfoCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
        
private:
    enum Consts
    {
        MIN_ARG_NUM = 1,
        MAX_ARG_NUM = 2
    };
};

class FlushCommand: public Command
{
public:
    
    enum CmdType
    {
        FLUSHDB,
        FLUSHALL
    };
    
    FlushCommand(CmdType type) { mCmdType = type; }
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 1,
        MAX_ARG_NUM = 1
    };
    
    CmdType mCmdType;
};

    
class ClientCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 3
    };
};

    
class ConfigCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 4
    };
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
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 1,
        MAX_ARG_NUM = 1
    };
    
    CmdType mCmdType;
};

    
};

#endif /* defined(__arag__ServerCmds__) */
