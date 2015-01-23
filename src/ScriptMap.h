#ifndef __arag__ScriptMap__
#define __arag__ScriptMap__

#include <unordered_map>
#include <string>

namespace arag
{
    
/*
    This class implements a map to store scripts and retrieve them when necessary.
 */
class ScriptMap
{
public:
    typedef std::unordered_map<std::string, std::string> ScriptMapType;
 
    bool exists(const std::string& script);
 
    void flush();
    
    std::string get(const std::string& hash);
    
    std::string set(const std::string& script);
    
private:
    
    ScriptMapType mScriptMap;
};
    
};


#endif /* defined(__arag__ScriptMap__) */
