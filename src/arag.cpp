#include <iostream>
#include "AragServer.h"
#include "SelfTest.h"
#include "RedisProtocol.h"
#include <string>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace std;
using namespace arag;

void report_errors(lua_State *L, int status)
{
    if (status != 0) {
        return;
    }

    std::cerr << "-- " << lua_tostring(L, -1) << std::endl;
    lua_pop(L, 1);
}

int redis_call(lua_State *L)
{
    return 1;
}

void luaLoadLib(lua_State *lua, const char *libname, lua_CFunction luafunc) {
    lua_pushcfunction(lua, luafunc);
    lua_pushstring(lua, libname);
    lua_call(lua, 1, 0);
}

void luaLoadLibraries(lua_State *lua) {
    luaLoadLib(lua, "", luaopen_base);
    luaLoadLib(lua, LUA_IOLIBNAME, luaopen_io);
    luaLoadLib(lua, LUA_TABLIBNAME, luaopen_table);
    luaLoadLib(lua, LUA_STRLIBNAME, luaopen_string);
    luaLoadLib(lua, LUA_MATHLIBNAME, luaopen_math);
    luaLoadLib(lua, LUA_DBLIBNAME, luaopen_debug);
}

void luaLoadRedisFunctions(lua_State *lua)
{
    lua_newtable(lua);
    lua_pushstring(lua,"call");
    lua_pushcfunction(lua, redis_call);
    lua_settable(lua,-3);
    
    lua_setglobal(lua,"redis");
}

int main(int argc, char* argv[])
{
//    {
//        const char* file = "../../../../../external/lua-5.1.5/test/test.lua";
//        
//        lua_State *L = lua_open();
//        
//        luaLoadLibraries(L);
//
//        luaLoadRedisFunctions(L);
//        
//        int s = luaL_loadfile(L, file);
//        
//        if ( s==0 ) {
//            s = lua_pcall(L, 0, LUA_MULTRET, 0);
//        }
//        
//        report_errors(L, s);
//        lua_close(L);
//        std::cerr << std::endl;
//        return 1;
//    }
    
    try {
        if (argc < 2)
        {
            std::cerr << "Usage: [server] | [test]" << std::endl;
            return 1;
        }
        
        if (string(argv[1]) == "server") {
            Arag::instance().startServer();
            return 1;
        }

        if (string(argv[1]) == "test") {
            SelfTest test;
            test.testAll();
            return 1;
        }

        std::cerr << "Usage: [server] | [test]" << std::endl;
    }
    catch (std::exception& e) {
        cout << "ERROR: " << e.what() << endl;
    }
    
    return 0;
}

