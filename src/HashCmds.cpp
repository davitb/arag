#include <sstream>
#include <climits>
#include <algorithm>
#include "HashCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

string HSetCommand::execute(InMemoryData& data)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string field = mTokens[2].first;
        string val = mTokens[3].first;
        
        StringMap& map = data.getFromHashMap(key);
        int ret = 0;
        
        switch (cmdType)
        {
            case SET:
            {
                try {
                    map.get(field);
                    ret = 0;
                }
                catch (...) {
                    ret = 1;
                }
                
                map.set(field, val);
                break;
            }
                
            case SETNX:
            {
                try {
                    map.get(field);
                    ret = 0;
                }
                catch (...) {
                    ret = 1;
                    map.set(field, val);
                }
                break;
            }
                
            case MSET:
            {
                size_t size = mTokens.size();
                
                if (mTokens.size() < Consts::MIN_ARG_NUM || size % 2 != 0) {
                    throw invalid_argument("Invalid args");
                }
                
                for (int i = 2; i < size; i += 2) {
                    map.set(mTokens[i].first, mTokens[i + 1].first);
                }
                
                return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
            }
        }
        
        return RedisProtocol::serializeNonArray(to_string(ret), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string HGetCommand::execute(InMemoryData& data)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string field = mTokens[2].first;
        
        StringMap& map = data.getFromHashMap(key);
        
        return RedisProtocol::serializeNonArray(map.get(field), RedisProtocol::DataType::BULK_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string HExistsCommand::execute(InMemoryData& data)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string field = mTokens[2].first;
        
        StringMap& map = data.getFromHashMap(key);
        int ret = 1;

        try {
            map.get(field);
        }
        catch (...) {
            ret = 0;
        }
        
        return RedisProtocol::serializeNonArray(to_string(ret), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}


//-------------------------------------------------------------------------

string HDelCommand::execute(InMemoryData& data)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        StringMap& map = data.getFromHashMap(key);
        int numDeleted = 0;
        
        for (int i = 1; i < mTokens.size(); ++i) {
            numDeleted += (map.deleteKey(mTokens[i].first) == true ? 1 : 0);
        }
        
        return RedisProtocol::serializeNonArray(to_string(numDeleted), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string HGetAllCommand::execute(InMemoryData& data)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        StringMap& map = data.getFromHashMap(key);

        switch (cmdType)
        {
            case GETALL:
            {
                return RedisProtocol::serializeArray(map.getAll(StringMap::KEYS_AND_VALUES));
            }
                
            case KEYS:
            {
                return RedisProtocol::serializeArray(map.getAll(StringMap::KEYS));
            }

            case VALS:
            {
                return RedisProtocol::serializeArray(map.getAll(StringMap::VALUES));
            }

            case MGET:
            {
                if (mTokens.size() == Consts::MIN_ARG_NUM) {
                    throw invalid_argument("Wrong number of args");
                }
                
                size_t numOfKeys = mTokens.size() - 2;
                vector<string> keys(numOfKeys);
                for (int i = 0; i < numOfKeys; ++i) {
                    keys[i] = mTokens[i + 2].first;
                }
                
                return RedisProtocol::serializeArray(map.mget(keys));
            }

            case LEN:
            {
                return RedisProtocol::serializeNonArray(to_string(map.len()),
                                                        RedisProtocol::DataType::INTEGER);
            }
        }
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string HIncrByCommand::execute(InMemoryData& data)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string field = mTokens[2].first;
        StringMap& map = data.getFromHashMap(key);
        
        switch (cmdType)
        {
            case INCRBY:
            {
                int by = Utils::convertToInt(mTokens[3].first);
                return RedisProtocol::serializeNonArray(to_string(map.incrBy(field, by)),
                                                        RedisProtocol::DataType::INTEGER);
            }
                
            case INCRBYFLOAT:
            {
                double by = Utils::convertToDouble(mTokens[3].first);
                return RedisProtocol::serializeNonArray(map.incrBy(field, by),
                                                        RedisProtocol::DataType::BULK_STRING);
            }
        }
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}
