#ifndef __arag__KeyCmds__
#define __arag__KeyCmds__

#include "Commands.h"

namespace arag
{
    
//----------------------------------------------------------------

class DelCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = INT_MAX
    };
};

//----------------------------------------------------------------

class ExistsCommand: public Command
{
public:
    
    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
    
private:
    enum Consts
    {
        MIN_ARG_NUM = 2,
        MAX_ARG_NUM = 2
    };
};
    
    
////----------------------------------------------------------------
//
//class : public Command
//{
//public:
//    
//    enum CmdType
//    {
//        RANGE,
//        REVRANGE
//    };
//    
//    ZRangeCommand(CmdType type) {mCmdType = type; }
//    
//    virtual std::string execute(InMemoryData& data, SessionContext& ctx);
//    
//private:
//    enum Consts
//    {
//        MIN_ARG_NUM = 4,
//        MAX_ARG_NUM = 5
//    };
//    
//    CmdType mCmdType;
//};

};


#endif /* defined(__arag__KeyCmds__) */
