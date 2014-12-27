#ifndef __CacheServer__Commands__
#define __CacheServer__Commands__

#include <string>
#include <vector>
#include <climits>
#include <memory>
#include "CSMap.h"
#include "SelfTest.h"

namespace arag
{

namespace command_const
{
    static const std::string CMD_INTERNAL_STOP = "internal::stop";
    static const std::string CMD_EXTERNAL_EXIT = "exit";
    static const std::string CMD_INTERNAL_CLEANUP = "internal::cleanup";
    static const std::string CMD_INTERNAL_PREFIX = "internal::";
};

static std::vector<std::string> sSupportedCommands = {
    "GET",
    "SET",
    "GETSET",
    "GETRANGE",
    "INCR",
    "MGET",
    "APPEND",
    "BITCOUNT",
    "BITOP",
    "BITPOS",
    "GETBIT",
    "SETBIT"
};

/*
    Base class for all commands.
 */
class Command
{
public:
    
    virtual std::string execute(CSMap& map) = 0;
    
    std::string getCommandName() const;
    
    static std::shared_ptr<Command> createCommand(std::string cmdline);

protected:
    
    void setTokens(const std::vector<std::pair<std::string, int>>& tokens);
    
    friend class SelfTest;
    
protected:
    std::vector<std::pair<std::string, int>> mTokens;
};

class InternalCommand: public Command
{
public:
    
    InternalCommand(std::string name);
    
    
    virtual std::string execute(CSMap& map)
    {
        throw std::logic_error("This function should not be called");
    }
};

class SetCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 5
    };
};

class GetCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 2
    };
};

class AppendCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 5
    };
};

class GetRangeCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4
    };
};

class GetSetCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 5
    };
};

class IncrCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 2
    };
};

class MGetCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = INT_MAX
    };
};

class BitCountCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 4
    };
};

class BitOpCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = INT_MAX
    };
};
    
class BitPosCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 5
    };
};

class GetBitCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 3
    };
};

class SetBitCommand: public Command
{
public:
    
    virtual std::string execute(CSMap& map);
    
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4,
        MAX_OFFSET = INT_MAX
    };
};
    
};

#endif /* defined(__CacheServer__Commands__) */
