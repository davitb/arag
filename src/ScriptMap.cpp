#include "ScriptMap.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <iostream>

using namespace std;
using namespace arag;

bool ScriptMap::exists(const std::string& hash)
{
    auto item = mScriptMap.find(hash);
    if (item == mScriptMap.end()) {
        return false;
    }
    return true;
}

void ScriptMap::flush()
{
    mScriptMap.clear();
}

std::string ScriptMap::get(const std::string& hash)
{
    if (!exists(hash)) {
        return "";
    }
    
    return mScriptMap[hash];
}

std::string ScriptMap::set(const std::string& script)
{
    string hash = Utils::sha1(script);
    mScriptMap[hash] = script;
    return hash;
}
