#ifndef __arag__SetCmds__
#define __arag__SetCmds__

#include "Commands.h"

namespace arag
{
    
class SAddCommand: public Command
{
public:
    
    DEEP_CLONE(SAddCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = INT_MAX
    };
};

class SMembersCommand: public Command
{
public:
    
    DEEP_CLONE(SMembersCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 2
    };
};

class SCardCommand: public Command
{
public:
    
    enum CmdType
    {
        CARD,
        ISMEMBER
    };
    
    SCardCommand(CmdType type) { mCmdType = type; }
    
    DEEP_CLONE(SCardCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 3
    };
    
    CmdType mCmdType;
};

class SRemCommand: public Command
{
public:
    
    enum CmdType
    {
        REM,
        POP
    };
    
    SRemCommand(CmdType type) { mCmdType = type; }
    
    DEEP_CLONE(SRemCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType mCmdType;
};

class SDiffCommand: public Command
{
public:
    
    enum CmdType
    {
        DIFF,
        DIFFSTORE
    };
    
    SDiffCommand(CmdType type) { mCmdType = type; }
    
    DEEP_CLONE(SDiffCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType mCmdType;
};

class SInterCommand: public Command
{
public:
    
    enum CmdType
    {
        INTER,
        INTERSTORE
    };
    
    SInterCommand(CmdType type) { mCmdType = type; }
    
    DEEP_CLONE(SInterCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType mCmdType;
};

class SUnionCommand: public Command
{
public:
    
    enum CmdType
    {
        UNION,
        UNIONSTORE
    };
    
    SUnionCommand(CmdType type) { mCmdType = type; }
    
    DEEP_CLONE(SUnionCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = INT_MAX
    };
    
    CmdType mCmdType;
};

class SMoveCommand: public Command
{
public:
    
    DEEP_CLONE(SMoveCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4
    };
};

class SRandMemberCommand: public Command
{
public:
    
    DEEP_CLONE(SRandMemberCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 3
    };
};

};

#endif /* defined(__arag__SetCmds__) */
