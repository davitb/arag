#include "LuaInterpreter.h"
#include "RedisProtocol.h"
#include <iostream>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
    
LUALIB_API int (luaopen_cjson) (lua_State *L);
LUALIB_API int (luaopen_struct) (lua_State *L);
LUALIB_API int (luaopen_cmsgpack) (lua_State *L);
LUALIB_API int (luaopen_bit) (lua_State *L);
}

enum
{
    ARAG_DEBUG,
    ARAG_VERBOSE,
    ARAG_NOTICE,
    ARAG_WARNING
};


using namespace std;
using namespace arag;

class LuaInterpreter::impl
{
public:

    static int redisCallImpl(lua_State* L, bool bForceExitLua)
    {
        if (spHandler) {
            int argc = lua_gettop(L);
            vector<string> tokens;
            
            for (int n = 1; n <= argc; ++n) {
                tokens.push_back(string(lua_tostring(L, n)));
            }
            
            try {
                CommandResultPtr result = spHandler->onRedisCall(tokens);
                if (bForceExitLua) {
                    if (result->getType() == CommandResult::SINGLE_RESPONSE) {
                        if (result->getSingleResult().second == RedisProtocol::ERROR) {
                            pushErrorToLua(L, string("Redis command failed with: ") +
                                           result->getSingleResult().first);
                            return 0;
                        }
                    }
                }
                CommandResultToLuaType(L, result);
                
            }
            catch (exception& e) {
                if (bForceExitLua) {
                    pushErrorToLua(L, string("Redis command failed with: ") + e.what());
                    return 0;
                }
                else {
                    CommandResultToLuaType(L, CommandResult::redisErrorResult(e.what()));
                }
            }
            return 1;
        }
        return 0;
    }
    
    static int redisCall(lua_State *L)
    {
        return redisCallImpl(L, false);
    }
    
    static int redisPCall(lua_State *L)
    {
        return redisCallImpl(L, true);
    }

    static int redisLog(lua_State *L)
    {
        return 1;
    }

    static int pushSingleFieldTable(lua_State *lua, const string& status)
    {
        if (lua_gettop(lua) != 1 || lua_type(lua,-1) != LUA_TSTRING) {
            pushErrorToLua(lua, "wrong number or type of arguments");
            return 1;
        }
        
        lua_newtable(lua);
        lua_pushstring(lua, "err");
        lua_pushvalue(lua, -3);
        lua_settable(lua, -3);
        return 1;
    }
    
    static int redisErrorReply(lua_State *lua)
    {
        return pushSingleFieldTable(lua, "err");
    }

    static int redisStatusReply(lua_State *lua)
    {
        return pushSingleFieldTable(lua, "ok");
    }
    
    static int sha1hex(lua_State *L)
    {
        int argc = lua_gettop(L);
        if (argc != 1) {
            pushErrorToLua(L, "wrong number of arguments");
            return 1;
        }
        
        size_t len = 0;
        string source = string(lua_tolstring(L, 1, &len));
        string hex = Utils::sha1(source);
        lua_pushstring(L, hex.c_str());
        return 1;
    }
    
    static void pushErrorToLua(lua_State* lua, const string& error) {
        lua_newtable(lua);
        lua_pushstring(lua, "err");
        lua_pushstring(lua, error.c_str());
    }
    
    void loadLib(const char *libname, lua_CFunction luafunc)
    {
        lua_pushcfunction(lua, luafunc);
        lua_pushstring(lua, libname);
        lua_call(lua, 1, 0);
    }
    
    void loadLibraries()
    {
        loadLib("", luaopen_base);
        loadLib(LUA_TABLIBNAME, luaopen_table);
        loadLib(LUA_STRLIBNAME, luaopen_string);
        loadLib(LUA_MATHLIBNAME, luaopen_math);
        loadLib(LUA_DBLIBNAME, luaopen_debug);
        loadLib("cjson", luaopen_cjson);
        loadLib("struct", luaopen_struct);
        loadLib("cmsgpack", luaopen_cmsgpack);
        loadLib("bit", luaopen_bit);
    }
    
    void loadRedisFunctions()
    {
        lua_newtable(lua);
        lua_pushstring(lua, "call");
        lua_pushcfunction(lua, redisCall);
        lua_settable(lua, -3);

        lua_pushstring(lua, "pcall");
        lua_pushcfunction(lua, redisPCall);
        lua_settable(lua, -3);
        
        lua_pushstring(lua, "error_reply");
        lua_pushcfunction(lua, redisErrorReply);
        lua_settable(lua, -3);
        
        lua_pushstring(lua, "status_reply");
        lua_pushcfunction(lua, redisStatusReply);
        lua_settable(lua, -3);
        
        lua_pushstring(lua, "log");
        lua_pushcfunction(lua, redisLog);
        lua_settable(lua, -3);
        
        lua_pushstring(lua, "LOG_DEBUG");
        lua_pushnumber(lua, ARAG_DEBUG);
        lua_settable(lua, -3);
        
        lua_pushstring(lua, "LOG_VERBOSE");
        lua_pushnumber(lua, ARAG_VERBOSE);
        lua_settable(lua, -3);
        
        lua_pushstring(lua, "LOG_NOTICE");
        lua_pushnumber(lua, ARAG_NOTICE);
        lua_settable(lua, -3);
        
        lua_pushstring(lua, "LOG_WARNING");
        lua_pushnumber(lua, ARAG_WARNING);
        lua_settable(lua, -3);
        
        lua_pushstring(lua, "sha1hex");
        lua_pushcfunction(lua, sha1hex);
        lua_settable(lua, -3);
        
        lua_setglobal(lua,"redis");
    }
    
    void removeUnsupportedFunctions() {
        lua_pushnil(lua);
        lua_setglobal(lua,"loadfile");
    }
    
    void setGlobalArray(const string& varName, const vector<string>& elems) {
        lua_newtable(lua);
        for (int i = 0; i < elems.size(); ++i) {
            lua_pushlstring(lua, elems[i].c_str(), elems[i].length());
            lua_rawseti(lua, -2, i + 1);
        }
        lua_setglobal(lua, varName.c_str());
    }
    
    CommandResultPtr luaReplyToCommandResult()
    {
        CommandResultPtr result;
        int t = lua_type(lua, -1);
        
        switch (t)
        {
            case LUA_TSTRING:
            {
                string reply;
                reply.append((char*)lua_tostring(lua,-1),lua_strlen(lua,-1));
                result = CommandResultPtr(new CommandResult(reply, RedisProtocol::BULK_STRING));
                break;
            }
                
            case LUA_TNUMBER:
            {
                result = CommandResultPtr(new CommandResult(to_string((int)lua_tonumber(lua, -1)),
                                                          RedisProtocol::INTEGER));
                break;
            }
                
            case LUA_TBOOLEAN:
            {
                if (lua_toboolean(lua, -1)) {
                    result = CommandResultPtr(new CommandResult("1", RedisProtocol::INTEGER));
                    
                }
                else {
                    result = CommandResult::redisNULLResult();
                }
                break;
            }

            case LUA_TTABLE:
            {
                lua_pushstring(lua, "err");
                lua_gettable(lua, -2);
                t = lua_type(lua, -1);
                if (t == LUA_TSTRING) {
                    string err(lua_tostring(lua, -1));
                    lua_pop(lua, 2);
                    result = CommandResultPtr(new CommandResult(err, RedisProtocol::ERROR));
                    break;
                }
                
                lua_pop(lua, 1);
                lua_pushstring(lua, "ok");
                lua_gettable(lua, -2);
                t = lua_type(lua, -1);
                if (t == LUA_TSTRING) {
                    string ok(lua_tostring(lua, -1));
                    lua_pop(lua, 2);
                    result = CommandResultPtr(new CommandResult(redis_const::OK,
                                                              RedisProtocol::SIMPLE_STRING));
                    break;
                }

                lua_pop(lua, 1); /* Discard the 'ok' field */
                
                // Parse table into CommandResult::MULT_RESPONSE
                
                result = CommandResultPtr(new CommandResult(CommandResult::MULTI_RESPONSE));
                int i = 1;
                
                while (true) {
                    lua_pushnumber(lua, i++);
                    lua_gettable(lua, -2);
                    t = lua_type(lua, -1);
                    if (t == LUA_TNIL) {
                        lua_pop(lua, 1);
                        break;
                    }
                    
                    result->appendToMultiArray(luaReplyToCommandResult());
                }
                break;
            }
                
            default:
            {
                result = CommandResult::redisNULLResult();
            }
                
        }
        
        lua_pop(lua,1);
        return result;
    }
    
    static void CommandResultToLuaType(lua_State* lua, CommandResultPtr cmd)
    {
        switch (cmd->getType())
        {
            case CommandResult::SINGLE_RESPONSE:
            {
                CommandResult::ResultSingle res = cmd->getSingleResult();
                
                switch (res.second)
                {
                    case RedisProtocol::INTEGER:
                    {
                        lua_pushnumber(lua, Utils::convertToInt(res.first));
                        break;
                    }
                        
                    case RedisProtocol::SIMPLE_STRING:
                    {
                        lua_newtable(lua);
                        lua_pushstring(lua, "ok");
                        lua_pushlstring(lua, res.first.c_str(), res.first.length());
                        lua_settable(lua, -3);
                        break;
                    }

                    case RedisProtocol::BULK_STRING:
                    {
                        if (res.first == redis_const::NULL_BULK_STRING) {
                            lua_pushboolean(lua, 0);
                            break;
                        }

                        cout << res.first << endl;
                        lua_pushlstring(lua, res.first.c_str(), res.first.length());
                        break;
                    }
                        
                    case RedisProtocol::ERROR:
                    {
                        lua_newtable(lua);
                        lua_pushstring(lua, "err");
                        lua_pushlstring(lua, res.first.c_str(), res.first.length());
                        lua_settable(lua, -3);
                        break;
                    }
                        
                    case RedisProtocol::NILL:
                    {
                        lua_pushboolean(lua, 0);
                        break;
                    }
                }
                
                break;
            }
                
            case CommandResult::MULTI_RESPONSE:
            {
                CommandResult::ResultMultiCommand cmds = cmd->getMultiCommandResult();
                
                for (int i = 0; i < cmds.size(); ++i) {
                    lua_pushnumber(lua, i + 1);
                    CommandResultToLuaType(lua, cmds[i]);
                    lua_settable(lua, -3);
                }
                
                break;
            }
                
            default:
            {
                break;
            }
        }
    }
    
    void init()
    {
        lua = lua_open();
        loadLibraries();
        loadRedisFunctions();
        removeUnsupportedFunctions();
    }

    void uninit()
    {
        lua_close(lua);
    }

    CommandResultPtr runScript(const std::string& script,
                               const std::vector<std::string>& keys,
                               const std::vector<std::string>& args,
                               IScriptNotifications &handler)
    {
        setGlobalArray("KEYS", keys);
        setGlobalArray("ARGV", args);
        
        int status = luaL_loadstring(lua, script.c_str());
        if (status != 0) {
            throw EScriptFailed();
        }
        
        string s = script;
        spHandler = &handler;
        status = lua_pcall(lua, 0, LUA_MULTRET, 0);
        if (status != 0) {
            throw EScriptFailed();
        }

        spHandler = nullptr;
        return luaReplyToCommandResult();
    }

private:
    lua_State* lua;
    static IScriptNotifications* spHandler;
};

IScriptNotifications* LuaInterpreter::impl::spHandler = nullptr;

LuaInterpreter::LuaInterpreter() : pimpl(new impl)
{
    pimpl->init();
}

LuaInterpreter::~LuaInterpreter()
{
    pimpl->uninit();
}

CommandResultPtr LuaInterpreter::runScript(const std::string& script,
                                           const std::vector<std::string>& keys,
                                           const std::vector<std::string>& args,
                                           IScriptNotifications &handler)
{
    return pimpl->runScript(script, keys, args, handler);
}

