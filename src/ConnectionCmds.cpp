#include "ConnectionCmds.h"
#include "RedisProtocol.h"

using namespace std;
using namespace arag;

//-------------------------------------------------------------------------

string PingCommand::execute(InMemoryData& data)
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
                
            case ECHO:
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
