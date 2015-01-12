#include "HLLCmds.h"
#include "KeyCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

string PFAddCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int bAdded = 0;
        HLLMap& hll = db.getHyperLogLogMap();
        
        for (int i = 2; i < mTokens.size(); ++i) {
            bAdded += hll.add(key, mTokens[i].first);
        }
        
        if (bAdded != 0) {
            bAdded = 1;
        }
        
        return RedisProtocol::serializeNonArray(to_string(bAdded), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string PFCountCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        HLLMap& hll = db.getHyperLogLogMap();
        int count = 0;
        
        if (cmdNum == Consts::MIN_ARG_NUM) {
            count = hll.count(mTokens[1].first);
        }
        else {
            vector<string> keys;
            for (int i = 1; i < mTokens.size(); ++i) {
                keys.push_back(mTokens[i].first);
            }
            count = hll.count(keys);
        }
        
        return RedisProtocol::serializeNonArray(to_string(count), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string PFMergeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string destKey = mTokens[1].first;
        HLLMap& hll = db.getHyperLogLogMap();
        
        vector<string> keys;
        for (int i = 2; i < mTokens.size(); ++i) {
            keys.push_back(mTokens[i].first);
        }

        hll.merge(destKey, keys);
        
        return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

