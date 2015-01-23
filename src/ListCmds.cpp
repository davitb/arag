#include <climits>
#include <algorithm>
#include "ListCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "Database.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

CommandResultPtr LPushCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        
        ListMap& listMap = data.getListMap();
        int len = 0;
        
        switch (mCmdType)
        {
            case RPUSH:
            {
                
                for (int i = 2; i < mTokens.size(); ++i) {
                    len = listMap.push(key, mTokens[i].first, ListMap::Position::BACK);
                }
                
                FIRE_EVENT(EventPublisher::Event::rpush, key);
                break;
            }
                
            case RPUSHX:
            {
                if (listMap.keyExists(key)) {
                    const string& value = mTokens[2].first;
                    len = listMap.push(key, value, ListMap::Position::BACK);
                    FIRE_EVENT(EventPublisher::Event::rpush, key);
                }
                break;
            }
                
            case LPUSH:
            {
                for (int i = 2; i < mTokens.size(); ++i) {
                    len = listMap.push(key, mTokens[i].first, ListMap::Position::FRONT);
                }
                
                FIRE_EVENT(EventPublisher::Event::lpush, key);
                break;
            }

            case LPUSHX:
            {
                if (listMap.keyExists(key)) {
                    const string& value = mTokens[2].first;
                    len = listMap.push(key, value, ListMap::Position::FRONT);
                    FIRE_EVENT(EventPublisher::Event::lpush, key);                    
                }
            
                break;
            }
        }
        
        return CommandResultPtr(new CommandResult(to_string(len),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr LGetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return CommandResultPtr(new CommandResult(redis_const::KEY_DOESNT_EXIST,
                                                      RedisProtocol::ERROR));
        }
        
        switch (mCmdType)
        {
            case LEN:
            {
                return CommandResultPtr(new CommandResult(to_string(listMap.size(key)),
                                     RedisProtocol::INTEGER));
            }
                
            case INDEX:
            {
                if (cmdNum != Consts::MAX_ARG_NUM) {
                    throw EInvalidArgument();
                }
                int index = Utils::convertToInt(mTokens[2].first);
                
                string val = listMap.val(key, index);
                
                return CommandResultPtr(new CommandResult(val, RedisProtocol::BULK_STRING));
            }
        }
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr LRemCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return CommandResult::redisNULLResult();
        }
        
        switch (mCmdType)
        {
            case RPOP:
            {
                string res = listMap.pop(key, ListMap::Position::BACK);
                
                if (!listMap.keyExists(key)) {
                    FIRE_EVENT(EventPublisher::Event::del, key);
                }
                FIRE_EVENT(EventPublisher::Event::rpop, key);
                
                return CommandResultPtr(new CommandResult(res, RedisProtocol::BULK_STRING));
            }
                
            case LPOP:
            {
                string res = listMap.pop(key, ListMap::Position::FRONT);

                if (!listMap.keyExists(key)) {
                    FIRE_EVENT(EventPublisher::Event::del, key);
                }
                FIRE_EVENT(EventPublisher::Event::lpop, key);

                return CommandResultPtr(new CommandResult(res, RedisProtocol::BULK_STRING));
            }

            case RPOPLPUSH:
            {
                if (cmdNum != Consts::MAX_ARG_NUM - 1) {
                    throw EInvalidArgument();
                }
                const string& dest = mTokens[2].first;
                
                string val = listMap.pop(key, ListMap::Position::BACK);
                if (!listMap.keyExists(key)) {
                    FIRE_EVENT(EventPublisher::Event::del, key);
                }
                FIRE_EVENT(EventPublisher::Event::rpop, key);
                
                listMap.push(dest, val, ListMap::Position::FRONT);
                FIRE_EVENT(EventPublisher::Event::lpush, key);
                
                return CommandResultPtr(new CommandResult(val, RedisProtocol::BULK_STRING));
            }
                
            case REM:
            {
                if (cmdNum != Consts::MAX_ARG_NUM) {
                    throw EInvalidArgument();
                }
                
                int count = Utils::convertToInt(mTokens[2].first);
                const string& val = mTokens[3].first;
                
                count = listMap.rem(key, val, count);
                
                return CommandResultPtr(new CommandResult(to_string(count), RedisProtocol::INTEGER));
            }
        }
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr LRangeCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        int start = Utils::convertToInt(mTokens[2].first);
        int end = Utils::convertToInt(mTokens[3].first);
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return CommandResult::redisNULLResult();
        }
        
        return CommandResultPtr(new CommandResult(listMap.getRange(key, start, end)));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr LSetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        int index = Utils::convertToInt(mTokens[2].first);
        const string& val = mTokens[3].first;
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return CommandResult::redisNULLResult();
        }
        
        listMap.setVal(key, index, val);
        
        FIRE_EVENT(EventPublisher::Event::lset, key);
        
        return CommandResult::redisOKResult();
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr LTrimCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        int start = Utils::convertToInt(mTokens[2].first);
        int stop = Utils::convertToInt(mTokens[3].first);
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return CommandResult::redisNULLResult();
        }
        
        listMap.trim(key, start, stop);
        
        if (!listMap.keyExists(key)) {
            FIRE_EVENT(EventPublisher::Event::del, key);
        }
        FIRE_EVENT(EventPublisher::Event::ltrim, key);
        
        return CommandResult::redisOKResult();
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr LInsertCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        const string& pos = mTokens[2].first;
        if (pos != "BEFORE" && pos != "AFTER") {
            throw EInvalidArgument();
        }
        const string& pivot = mTokens[3].first;
        const string& val = mTokens[4].first;
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return CommandResult::redisNULLResult();
        }
        
        int len = listMap.insert(key, pos, pivot, val);
        
        FIRE_EVENT(EventPublisher::Event::linsert, key);
        
        return CommandResultPtr(new CommandResult(to_string(len), RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr BLCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        int timeout = Utils::convertToInt(mTokens[mTokens.size() - 1].first);
        string res;
        list<string> watchKeys;
        
        ListMap& listMap = data.getListMap();

        // If timeout expired - return nill
        if (timeout != 0 && (mTimestamp + timeout < time(0))) {
            return CommandResultPtr(new CommandResult({
                make_pair(redis_const::NULL_BULK_STRING, RedisProtocol::BULK_STRING)
            }));
        }
        
        for (int i = 1; i < mTokens.size() - 1; ++i) {
            const std::string& key = mTokens[i].first;
            if (listMap.keyExists(key) && listMap.size(key) != 0) {
                
                // Found a non-empty list. Pop it up and return.
                
                switch (mCmdType)
                {
                    case BLPOP:
                        res = listMap.pop(key, ListMap::Position::FRONT);
                        break;
                        
                    case BRPOP:
                        res = listMap.pop(key, ListMap::Position::BACK);
                        break;
                }
                
                RedisProtocol::RedisArray arr = {
                    make_pair(key, RedisProtocol::BULK_STRING),
                    make_pair(res, RedisProtocol::BULK_STRING)
                };
                return CommandResultPtr(new CommandResult(arr));
            }
            watchKeys.push_back(key);
        }
        
        // Since no value exists in the list - set the pending BL command
        // to context for later execution
        ctx.setPendingtBLCommand(std::shared_ptr<Command>(this->clone()),
                                 timeout,
                                 watchKeys);
        
        return CommandResultPtr(new CommandResult(CommandResult::NO_RESPONSE));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr BRPopLPushCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const std::string& source = mTokens[1].first;
        const std::string& dest = mTokens[2].first;
        int timeout = Utils::convertToInt(mTokens[3].first);
        list<string> watchKeys;
        
        ListMap& listMap = data.getListMap();
        
        if (timeout != 0 && (mTimestamp + timeout < time(0))) {
            return CommandResultPtr(new CommandResult({
                make_pair(redis_const::NULL_BULK_STRING, RedisProtocol::BULK_STRING)
            }));
        }
        
        if (listMap.keyExists(source) && listMap.size(source) != 0) {
            // Found a non-empty list. Pop it up and return.
            string val = listMap.pop(source, ListMap::Position::BACK);
            if (!listMap.keyExists(source)) {
                FIRE_EVENT(EventPublisher::Event::del, source);
            }
            FIRE_EVENT(EventPublisher::Event::rpop, source);
            
            listMap.push(dest, val, ListMap::Position::FRONT);
            FIRE_EVENT(EventPublisher::Event::lpush, dest);
            
            return CommandResultPtr(new CommandResult(val, RedisProtocol::BULK_STRING));
        }

        watchKeys.push_back(source);
        
        // Since no value exists in the list - set the pending BL command
        // to context for later execution        
        ctx.setPendingtBLCommand(std::shared_ptr<Command>(this->clone()),
                                 timeout,
                                 watchKeys);
        
        return CommandResultPtr(new CommandResult(CommandResult::NO_RESPONSE));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}
