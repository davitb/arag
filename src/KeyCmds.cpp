#include "stdlib.h"
#include <algorithm>
#include "KeyCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

string DelCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        if (mTokens.size() % 2 != 0) {
            throw invalid_argument("Invalid args");
        }
        
        int numRemoved = 0;
        
        for (int i = 1; i < mTokens.size(); ++i) {
            numRemoved += db.delKey(mTokens[i].first);
        }
        
        return RedisProtocol::serializeNonArray(to_string(numRemoved), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string ExistsCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        string key = mTokens[1].first;
        
        int ret = db.keyExists(key) ? 1 : 0;
        
        return RedisProtocol::serializeNonArray(to_string(ret), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}
