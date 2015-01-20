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

CommandResultPtr HSetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
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
                
                FIRE_EVENT(EventPublisher::Event::hset, key);
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
                
                FIRE_EVENT(EventPublisher::Event::hset, key);
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
                
                FIRE_EVENT(EventPublisher::Event::hset, key);
                
                return CommandResultPtr(new CommandResult("OK", RedisProtocol::DataType::SIMPLE_STRING));
            }
        }

        return CommandResultPtr(new CommandResult(to_string(ret), RedisProtocol::DataType::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr HGetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string field = mTokens[2].first;
        
        StringMap& map = data.getFromHashMap(key);
     
        return CommandResultPtr(new CommandResult(map.get(field), RedisProtocol::DataType::BULK_STRING));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr HExistsCommand::execute(InMemoryData& data, SessionContext& ctx)
{
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
        
        return CommandResultPtr(new CommandResult(to_string(ret), RedisProtocol::DataType::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}


//-------------------------------------------------------------------------

CommandResultPtr HDelCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        StringMap& map = data.getFromHashMap(key);
        int numDeleted = 0;
        
        for (int i = 1; i < mTokens.size(); ++i) {
            numDeleted += map.delKey(mTokens[i].first);
        }
        
        if (map.keyExists(key) == 0) {
            FIRE_EVENT(EventPublisher::Event::del, key);
        }
        FIRE_EVENT(EventPublisher::Event::hdel, key);
        
        return CommandResultPtr(new CommandResult(to_string(numDeleted), RedisProtocol::DataType::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr HGetAllCommand::execute(InMemoryData& data, SessionContext& ctx)
{
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
                return CommandResultPtr(new CommandResult(map.getAll(StringMap::KEYS_AND_VALUES)));
            }
                
            case KEYS:
            {
                return CommandResultPtr(new CommandResult(map.getAll(StringMap::KEYS)));
            }

            case VALS:
            {
                return CommandResultPtr(new CommandResult(map.getAll(StringMap::VALUES)));
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

                return CommandResultPtr(new CommandResult(map.mget(keys)));
            }

            case LEN:
            {
                return CommandResultPtr(new CommandResult(to_string(map.size()), RedisProtocol::DataType::INTEGER));
            }
        }
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr HIncrByCommand::execute(InMemoryData& data, SessionContext& ctx)
{
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
                
                int res = map.incrBy(field, by);
                
                FIRE_EVENT(EventPublisher::Event::hincrby, key);
                
                return CommandResultPtr(new CommandResult(to_string(res), RedisProtocol::DataType::INTEGER));
            }
                
            case INCRBYFLOAT:
            {
                double by = Utils::convertToDouble(mTokens[3].first);
                
                string res = map.incrBy(field, by);
                
                FIRE_EVENT(EventPublisher::Event::hincrbyfloat, key);

                return CommandResultPtr(new CommandResult(res, RedisProtocol::DataType::INTEGER));
            }
        }
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}
