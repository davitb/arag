#ifndef arag_StringCmds_h
#define arag_StringCmds_h

#include "Commands.h"

namespace arag
{
    
class SetCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
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
    
    virtual std::string execute(InMemoryData& data);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 3,
        MAX_ARG_NUM = 3
    };
};

};

#endif
