#include "ConnectionCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;

//-------------------------------------------------------------------------

CommandResultPtr PingCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        switch (mCmdType)
        {
            case PING:
            {
                return CommandResultPtr(new CommandResult("PONG", RedisProtocol::DataType::SIMPLE_STRING));
            }
                
            case ECHO_CMD:
            {
                if (cmdNum != Consts::MAX_ARG_NUM) {
                    throw invalid_argument("Invalid args");
                }
                return CommandResultPtr(new CommandResult(mTokens[1].first, RedisProtocol::DataType::BULK_STRING));
            }
        }
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SelectCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        int index = Utils::convertToInt(mTokens[1].first);
        
        if (index < 0 || index >= Config::DATABASE_COUNT) {
                return CommandResultPtr(new CommandResult("Invalid index", RedisProtocol::DataType::ERROR));
        }
        
        ctx.setDatabaseIndex(index);

        return CommandResultPtr(new CommandResult("OK", RedisProtocol::DataType::SIMPLE_STRING));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}
