#ifndef arag_StringCmds_h
#define arag_StringCmds_h

#include "Commands.h"

namespace arag
{
 
    COMMAND_CLASS(SetCommand, 3, 6);
    
    COMMAND_CLASS(GetCommand, 2, 2);
    
    COMMAND_CLASS(AppendCommand, 3, 5);
    
    COMMAND_CLASS(GetRangeCommand, 4, 4);
    
    COMMAND_CLASS(GetSetCommand, 3, 5);
    
    COMMAND_CLASS(IncrCommand, 2, 2);
    
    COMMAND_CLASS(MGetCommand, 2, INT_MAX);
    
    COMMAND_CLASS(BitCountCommand, 2, 4);
    
    COMMAND_CLASS(BitOpCommand, 4, INT_MAX);
    
    COMMAND_CLASS(BitPosCommand, 3, 5);
    
    COMMAND_CLASS(GetBitCommand, 3, 3);
    
    COMMAND_CLASS(StrlenCommand, 2, 2);
    
    COMMAND_CLASS(IncrByCommand, 3, 3);
    
    COMMAND_CLASS(IncrByFloatCommand, 3, 3);
    
    COMMAND_CLASS(DecrCommand, 2, 2);
    
    COMMAND_CLASS(DecrByCommand, 3, 3);
    
    
    class SetRangeCommand: public Command
    {
    public:
        
        DEEP_CLONE(SetRangeCommand)
        
        virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
        
    private:
        enum Consts
        {
            MIN_ARG_NUM = 4,
            MAX_ARG_NUM = 4,
            MAX_VALUE = 512 * 1024 * 1024
        };
    };
    
    class MSetCommand: public Command
    {
    public:
        
        MSetCommand(bool msetNX) { mNX = msetNX; };
        
        DEEP_CLONE(MSetCommand)
        
        virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
        
    private:
        enum Consts
        {
            MIN_ARG_NUM = 3,
            MAX_ARG_NUM = INT_MAX
        };
        
    private:
        bool mNX;
    };
    
    class SetBitCommand: public Command
    {
    public:
        
        DEEP_CLONE(SetBitCommand)
        
        virtual CommandResultPtr execute(InMemoryData& data, SessionContext& ctx);
        
        enum Consts
        {
            MIN_ARG_NUM = 4,
            MAX_ARG_NUM = 4,
            MAX_OFFSET = INT_MAX
        };
    };
    
};

#endif
