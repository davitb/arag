#include "ScriptCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "AragServer.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

class Handler : public IScriptNotifications
{
public:
    
    Handler(InMemoryData& db, SessionContext& ctx) : mDB(db), mCtx(ctx)
    {
    }
    
    virtual CommandResultPtr onRedisCall(const std::vector<std::string>& tokens)
    {
        CommandResult::ResultArray arr;
        for (auto token : tokens) {
            arr.push_back(make_pair(token, RedisProtocol::BULK_STRING));
        }
        
        CommandResultPtr response = CommandResultPtr(new CommandResult(CommandResult::MULTI_RESPONSE));
        
        Command::executeEndToEnd(Command::getCommand(arr),
                                 mCtx.get().getSessionID(),
                                 response.get());
        
        return response->getMultiCommandResult()[0];
    }
    
    virtual CommandResultPtr onRedisPcall(const std::vector<std::string>& tokens)
    {
        return CommandResult::redisOKResult();
    }
    
private:
    reference_wrapper<SessionContext> mCtx;
    reference_wrapper<InMemoryData> mDB;
};


//-------------------------------------------------------------------------

CommandResultPtr EvalCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& script = mTokens[1].first;
        int numKeys = Utils::convertToInt(mTokens[2].first);
        
        if (3 + numKeys > cmdNum) {
            throw EInvalidArgument();
        }
        
        vector<string> keys;
        vector<string> args;
        
        for (int i = 3; i < 3 + numKeys; ++i) {
            keys.push_back(mTokens[i].first);
        }

        for (int i = 3 + numKeys; i < mTokens.size(); ++i) {
            args.push_back(mTokens[i].first);
        }
        
        LuaInterpreter& lua = Arag::instance().getLuaInterpreter();
        Handler handler(db, ctx);
        
        int selectedDB = ctx.getDatabaseIndex();
        
        CommandResultPtr res = lua.runScript(script, keys, args, handler);
        
        ctx.setDatabaseIndex(selectedDB);
        
        return res;
    }
    catch (std::exception& e) {
        return CommandResult::redisErrorResult(e.what());
    }
}

//-------------------------------------------------------------------------

CommandResultPtr EvalSHACommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        ScriptMap& map = db.getScriptMap();
        
        mTokens[0].first = "EVAL";
        mTokens[1].first = map.get(mTokens[1].first);
        if (mTokens[1].first.length() == 0) {
            return CommandResult::redisErrorResult(redis_const::ERR_NO_SCRIPT);
        }
        
        return Command::getCommand(mTokens)->execute(db, ctx);
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr ScriptCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& subCmd = mTokens[1].first;

        ScriptMap& map = db.getScriptMap();
        
        if (subCmd == "LOAD") {
            if (cmdNum <= 2) {
                throw EInvalidArgument();
            }
            
            const string& script = mTokens[2].first;

            return CommandResultPtr(new CommandResult(map.set(script),
                                                      RedisProtocol::BULK_STRING));
        }
        
        if (subCmd == "EXISTS") {
            if (cmdNum <= 2) {
                throw EInvalidArgument();
            }

            RedisProtocol::RedisArray arr;
            for (int i = 2; i < mTokens.size(); ++i) {
                int exists = map.exists(mTokens[i].first) ? 1 : 0;
                arr.push_back(make_pair(to_string(exists), RedisProtocol::INTEGER));
            }
            
            return CommandResultPtr(new CommandResult(arr));
        }
        
        if (subCmd == "FLUSH") {
            map.flush();
            return CommandResult::redisOKResult();
        }
        
        if (subCmd == "KILL") {
            // TODO: Kill the currently executing script
            return CommandResult::redisOKResult();
        }
    }
    catch (std::exception& e) {
    }
    
    return CommandResult::redisNULLResult();
}
