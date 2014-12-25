#ifndef __CacheServer__Commands__
#define __CacheServer__Commands__

#include <string>
#include <vector>
#include <climits>
#include <memory>
#include "CSMap.h"
#include "SelfTest.h"

namespace cache_server
{

static const char* CMD_INTERNAL_STOP = "internal::stop";
static const char* CMD_EXTERNAL_EXIT = "exit";
static const char* CMD_INTERNAL_CLEANUP = "internal::cleanup";
static const char* CMD_INTERNAL_PREFIX = "internal::";

static std::vector<std::string> sSupportedCommands = {
    "GET",
    "SET",
    "GETSET",
    "GETRANGE",
    "INCR",
    "MGET",
    "APPEND"    
};

class Command
{
public:
    
    virtual std::string execute(CSMap& map) = 0;
    
    std::string getCommandName() const;
    
    static std::shared_ptr<Command> createCommand(std::string cmdline);
    
protected:
    
    static int convertToInt(std::string val);
    
    static std::vector<std::string> parse(std::string cmdLine);
    
    static void insertNewWords(std::vector<std::string>& tokens, std::string line);
    
    void setTokens(const std::vector<std::string>& tokens);
    
    std::string stringsToString(const std::vector<std::string>& vec);
    
    friend class SelfTest;
    
protected:
    std::vector<std::string> mTokens;
};

class InternalCommand: public Command
{
public:
    
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
    
};

#endif /* defined(__CacheServer__Commands__) */
