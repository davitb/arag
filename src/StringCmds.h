#ifndef arag_StringCmds_h
#define arag_StringCmds_h

#include "Commands.h"

namespace arag
{
    
class SetCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
    DEEP_CLONE(SetCommand)
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 6
    };
};

class GetCommand: public Command
{
public:
    
    DEEP_CLONE(GetCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
    
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
    
    DEEP_CLONE(AppendCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
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
    
    DEEP_CLONE(GetRangeCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4
    };
};

class SetRangeCommand: public Command
{
public:
    
    DEEP_CLONE(SetRangeCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4,
        MAX_VALUE = 512 * 1024 * 1024
    };
};

class GetSetCommand: public Command
{
public:
    
    DEEP_CLONE(GetSetCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
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
    
    DEEP_CLONE(IncrCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
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
    
    DEEP_CLONE(MGetCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = INT_MAX
    };
};

class MSetCommand: public Command
{
public:
    
    MSetCommand(bool msetNX) { mNX = msetNX; };
    
    DEEP_CLONE(MSetCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = INT_MAX
    };
    
private:
    bool mNX;
};

class BitCountCommand: public Command
{
public:
    
    DEEP_CLONE(BitCountCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
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
    
    DEEP_CLONE(BitOpCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
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
    
    DEEP_CLONE(BitPosCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
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
    
    DEEP_CLONE(GetBitCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
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
    
    DEEP_CLONE(SetBitCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
    enum Consts
    {
        MIN_ARG_NUM = 4,
        MAX_ARG_NUM = 4,
        MAX_OFFSET = INT_MAX
    };
};

class StrlenCommand: public Command
{
public:
    
    DEEP_CLONE(StrlenCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 2
    };
};

class IncrByCommand: public Command
{
public:
    
    DEEP_CLONE(IncrByCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 3
    };
};

class IncrByFloatCommand: public Command
{
public:
    
    DEEP_CLONE(IncrByFloatCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 3
    };
};

class DecrCommand: public Command
{
public:
    
    DEEP_CLONE(DecrCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 2
    };
};

class DecrByCommand: public Command
{
public:
    
    DEEP_CLONE(DecrByCommand)
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 3
    };
};

};

#endif
