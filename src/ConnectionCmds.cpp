#include "ConnectionCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;

//-------------------------------------------------------------------------

string PingCommand::execute(InMemoryData& data, SessionContext& ctx)
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
                return RedisProtocol::serializeNonArray("PONG", RedisProtocol::DataType::SIMPLE_STRING);
            }
                
            case ECHO_CMD:
            {
                if (cmdNum != Consts::MAX_ARG_NUM) {
                    throw invalid_argument("Invalid args");
                }
                return RedisProtocol::serializeNonArray(mTokens[1].first, RedisProtocol::DataType::BULK_STRING);
            }
        }
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string SelectCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        int index = Utils::convertToInt(mTokens[1].first);
        
        if (index < 0 || index >= Config::DATABASE_COUNT) {
            return RedisProtocol::serializeNonArray("Invalid index", RedisProtocol::DataType::ERROR);
        }
        
        ctx.setDatabaseIndex(index);
        
        return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}
