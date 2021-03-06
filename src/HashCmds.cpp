#include <sstream>
#include <climits>
#include <algorithm>
#include "HashCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "Database.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

CommandResultPtr HSetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        const string& field = mTokens[2].first;
        const string& val = mTokens[3].first;
        
        StringMap& map = data.getHashMap().get(key);
        
        bool newKey = map.keyExists(key) == false;
        
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
                    throw EInvalidArgument();
                }
                
                for (int i = 2; i < size; i += 2) {
                    map.set(mTokens[i].first, mTokens[i + 1].first);
                }
                
                FIRE_EVENT(EventPublisher::Event::hset, key);
                
                if (newKey) {
                    FIRE_EVENT(EventPublisher::Event::hash_new, key);
                }
                
                return CommandResult::redisOKResult();
            }
        }

        if (newKey) {
            FIRE_EVENT(EventPublisher::Event::hash_new, key);
        }
                
        return CommandResultPtr(new CommandResult(to_string(ret), RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING,
                                                  RedisProtocol::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr HGetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        const string& field = mTokens[2].first;
        
        StringMap& map = data.getHashMap().get(key);
     
        return CommandResultPtr(new CommandResult(map.get(field),
                                                  RedisProtocol::BULK_STRING));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr HExistsCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        const string& field = mTokens[2].first;
        
        StringMap& map = data.getHashMap().get(key);
        int ret = 1;

        try {
            map.get(field);
        }
        catch (...) {
            ret = 0;
        }
        
        return CommandResultPtr(new CommandResult(to_string(ret),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}


//-------------------------------------------------------------------------

CommandResultPtr HDelCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        StringMap& map = data.getHashMap().get(key);
        int numDeleted = 0;
        
        for (int i = 1; i < mTokens.size(); ++i) {
            numDeleted += map.delKey(mTokens[i].first);
        }
        
        if (map.keyExists(key) == 0) {
            FIRE_EVENT(EventPublisher::Event::del, key);
        }
        FIRE_EVENT(EventPublisher::Event::hdel, key);
        
        return CommandResultPtr(new CommandResult(to_string(numDeleted),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr HGetAllCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        StringMap& map = data.getHashMap().get(key);

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
                    throw EInvalidArgument();
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
                return CommandResultPtr(new CommandResult(to_string(map.size()),
                                                          RedisProtocol::INTEGER));
            }
        }
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr HIncrByCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        const string& field = mTokens[2].first;
        StringMap& map = data.getHashMap().get(key);
        
        switch (cmdType)
        {
            case INCRBY:
            {
                int by = Utils::convertToInt(mTokens[3].first);
                
                int res = map.incrBy(field, by);
                
                FIRE_EVENT(EventPublisher::Event::hincrby, key);
                
                return CommandResultPtr(new CommandResult(to_string(res),
                                                          RedisProtocol::INTEGER));
            }
                
            case INCRBYFLOAT:
            {
                double by = Utils::convertToDouble(mTokens[3].first);
                
                string res = map.incrBy(field, by);
                
                FIRE_EVENT(EventPublisher::Event::hincrbyfloat, key);

                return CommandResultPtr(new CommandResult(res,
                                                          RedisProtocol::INTEGER));
            }
        }
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr HScanCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        int cursor = Utils::convertToInt(mTokens[2].first);
        string pattern = "";
        int count = 10; // Return max 10 items
        
        if (cmdNum > 3) {
            int pos = 3;
            if (mTokens[pos].first == "MATCH") {
                pattern = mTokens[pos + 1].first;
                pos += 2;
            }
            
            if (cmdNum > pos && mTokens[pos].first == "COUNT") {
                count = Utils::convertToInt(mTokens[pos + 1].first);
            }
        }
        
        if (cursor == 0) {
            ctx.setScanCommandStartTime(SessionContext::HSCAN);
        }
        
        HashMap& hmap = db.getHashMap();
        
        KeyMap::RedisArray secondArray;
        int nextCursor = hmap.scan(key,
                                   secondArray,
                                   pattern,
                                   cursor,
                                   ctx.getScanCommandStartTime(SessionContext::HSCAN),
                                   cursor + count);
        
        CommandResultPtr response(new CommandResult(CommandResult::MULTI_RESPONSE));
        
        response->appendToMultiArray(CommandResultPtr(new CommandResult(to_string(nextCursor), RedisProtocol::BULK_STRING)));
        response->appendToMultiArray(CommandResultPtr(new CommandResult(secondArray)));
        
        return response;
    }
    catch (EInvalidKey& e) {
        return CommandResult::redisErrorResult(e.what());
    }
    catch (std::exception& e) {
    }
    
    return CommandResult::redisNULLResult();
}

