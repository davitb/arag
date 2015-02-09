#ifndef __arag__SortedSetCmds__
#define __arag__SortedSetCmds__

#include "Commands.h"

namespace arag
{
    
//----------------------------------------------------------------

class ZRangeCommand: public Command
{
public:
    
    enum CmdType
    {
        RANGE,
        REVRANGE
    };
    
    ZRangeCommand(CmdType type) {mCmdType = type; }
    
    DEEP_CLONE(ZRangeCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 5
    };
    
    CmdType mCmdType;
};

//----------------------------------------------------------------

class ZRangeByLexCommand: public Command
{
public:
    
    enum CmdType
    {
        RANGEBYLEX,
        REVRANGEBYLEX
    };
    
    ZRangeByLexCommand(CmdType type) {mCmdType = type; }
    
    DEEP_CLONE(ZRangeByLexCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 7
    };
    
    CmdType mCmdType;
};

//----------------------------------------------------------------

class ZCountCommand: public Command
{
public:
    enum CmdType
    {
        COUNT,
        CARD,
        LEXCOUNT
    };
    
    ZCountCommand(CmdType type) {mCmdType = type; }
    
    DEEP_CLONE(ZCountCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 4
    };
    
    CmdType mCmdType;
};

//----------------------------------------------------------------

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
    
    DEEP_CLONE(ZScoreCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 3
    };
    
    CmdType mCmdType;
};

//----------------------------------------------------------------

class ZUnionCommand: public Command
{
public:
    
    enum CmdType
    {
        UNION,
        INTERSECT
    };
    
    ZUnionCommand(CmdType type) {mCmdType = type; }
    
    DEEP_CLONE(ZUnionCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType mCmdType;
};

//----------------------------------------------------------------

class ZRangeByCommand: public Command
{
public:
    
    enum CmdType
    {
        RANGEBYSCORE,
        REVRANGEBYSCORE
    };
    
    ZRangeByCommand(CmdType type) {mCmdType = type; }
    
    DEEP_CLONE(ZRangeByCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 8
    };
    
    CmdType mCmdType;
};

//----------------------------------------------------------------

class ZRemByCommand: public Command
{
public:
    
    enum CmdType
    {
        REMRANGEBYSCORE,
        REMRANGEBYRANK,
        REMRANGEBYLEX
    };
    
    ZRemByCommand(CmdType type) {mCmdType = type; }
    
    DEEP_CLONE(ZRemByCommand)
    
    virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4
    };
    
    CmdType mCmdType;
};

COMMAND_CLASS(ZRemCommand, 3, INT_MAX);
    
COMMAND_CLASS(ZAddCommand, 4, INT_MAX);

COMMAND_CLASS(ZIncrByCommand, 4, 4);

COMMAND_CLASS(ZScanCommand, 3, 7);
};


#endif /* defined(__arag__SortedSetCmds__) */
