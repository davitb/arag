#ifndef __arag__SortedSetCmds__
#define __arag__SortedSetCmds__

#include "Commands.h"

namespace arag
{
    
class ZAddCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = INT_MAX
    };
};

class ZRangeCommand: public Command
{
public:
    
    enum CmdType
    {
        RANGE,
        REVRANGE
    };
    
    ZRangeCommand(CmdType type) {mCmdType = type; }
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 5
    };
    
    CmdType mCmdType;
};

class ZCountCommand: public Command
{
public:
    enum CmdType
    {
        COUNT,
        CARD
    };
    
    ZCountCommand(CmdType type) {mCmdType = type; }
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 4
    };
    
    CmdType mCmdType;    
};
    
class ZScoreCommand: public Command
{
public:
    
    enum CmdType
    {
        SCORE,
        RANK,
        REVRANK
    };

    ZScoreCommand(CmdType type) {mCmdType = type; }
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 3
    };
    
    CmdType mCmdType;
};

class ZIncrByCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4
    };
};

class ZRemCommand: public Command
{
public:
    
    enum CmdType
    {
        REM,
        REMRANGEBYLEX,
        REMRANGEBYRANK,
        REMRANGEBYSCORE
    };
    
    ZRemCommand(CmdType type) { mCmdType = type; }
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType mCmdType;
};
    
};


#endif /* defined(__arag__SortedSetCmds__) */
