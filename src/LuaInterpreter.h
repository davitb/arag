#ifndef __arag__LuaInterpreter__
#define __arag__LuaInterpreter__

#include <string>
#include <vector>
#include "CommandResult.h"

namespace arag
{

class IScriptNotifications
{
public:
    
    virtual CommandResultPtr onRedisCall(const std::vector<std::string>& tokens) = 0;
    virtual CommandResultPtr onRedisPcall(const std::vector<std::string>& tokens) = 0;
};
    
class LuaInterpreter
{
public:

    LuaInterpreter();
    
    ~LuaInterpreter();
    
    CommandResultPtr runScript(const std::string& script,
                               const std::vector<std::string>& keys,
                               const std::vector<std::string>& args,
                               IScriptNotifications &handler);
    
private:
    class impl;
    std::unique_ptr<impl> pimpl;
};

};

#endif /* defined(__arag__LuaInterpreter__) */
