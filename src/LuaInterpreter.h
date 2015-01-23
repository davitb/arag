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
};
    
/*
    This class implement pImpl idiom and exposes Lua interpreter API to outside world.
*/
class LuaInterpreter
{
public:

    LuaInterpreter();
    
    ~LuaInterpreter();
    
    // Runs a script, fires redis call events when necessary and returns the result.
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
