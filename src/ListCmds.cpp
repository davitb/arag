#include <climits>
#include <algorithm>
#include "ListCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

string LPushCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
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
                    string value = mTokens[2].first;
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
                    string value = mTokens[2].first;
                    len = listMap.push(key, value, ListMap::Position::FRONT);
                    FIRE_EVENT(EventPublisher::Event::lpush, key);                    
                }
            
                break;
            }
        }
        
        return RedisProtocol::serializeNonArray(to_string(len), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string LGetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return RedisProtocol::serializeNonArray("Error: key doesn't exist",
                                                    RedisProtocol::DataType::ERROR);
        }
        
        switch (mCmdType)
        {
            case LEN:
            {
                return RedisProtocol::serializeNonArray(to_string(listMap.len(key)),
                                                        RedisProtocol::DataType::INTEGER);
            }
                
            case INDEX:
            {
                if (cmdNum != Consts::MAX_ARG_NUM) {
                    throw invalid_argument("Wrong number of arguments");
                }
                int index = Utils::convertToInt(mTokens[2].first);
                
                string val = listMap.val(key, index);
                
                return RedisProtocol::serializeNonArray(val, RedisProtocol::DataType::BULK_STRING);
            }
        }
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string LRemCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return redis_const::NULL_BULK_STRING;
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
                
                return RedisProtocol::serializeNonArray(res, RedisProtocol::DataType::BULK_STRING);
            }
                
            case LPOP:
            {
                string res = listMap.pop(key, ListMap::Position::FRONT);

                if (!listMap.keyExists(key)) {
                    FIRE_EVENT(EventPublisher::Event::del, key);
                }
                FIRE_EVENT(EventPublisher::Event::lpop, key);

                return RedisProtocol::serializeNonArray(res, RedisProtocol::DataType::BULK_STRING);
            }

            case RPOPLPUSH:
            {
                if (cmdNum != Consts::MAX_ARG_NUM - 1) {
                    throw invalid_argument("Invalid args");
                }
                string dest = mTokens[2].first;
                
                string val = listMap.pop(key, ListMap::Position::BACK);
                if (!listMap.keyExists(key)) {
                    FIRE_EVENT(EventPublisher::Event::del, key);
                }
                FIRE_EVENT(EventPublisher::Event::rpop, key);
                
                listMap.push(dest, val, ListMap::Position::FRONT);
                FIRE_EVENT(EventPublisher::Event::lpush, key);
                
                return RedisProtocol::serializeNonArray(val, RedisProtocol::DataType::BULK_STRING);
            }
                
            case REM:
            {
                if (cmdNum != Consts::MAX_ARG_NUM) {
                    throw invalid_argument("Invalid args");
                }
                
                int count = Utils::convertToInt(mTokens[2].first);
                string val = mTokens[3].first;
                
                count = listMap.rem(key, val, count);
                
                return RedisProtocol::serializeNonArray(to_string(count),
                                                        RedisProtocol::DataType::INTEGER);
            }
        }
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string LRangeCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int start = Utils::convertToInt(mTokens[2].first);
        int end = Utils::convertToInt(mTokens[3].first);
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return redis_const::NULL_BULK_STRING;
        }
        
        return RedisProtocol::serializeArray(listMap.getRange(key, start, end));
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string LSetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int index = Utils::convertToInt(mTokens[2].first);
        string val = mTokens[3].first;
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return redis_const::NULL_BULK_STRING;
        }
        
        listMap.setVal(key, index, val);
        
        FIRE_EVENT(EventPublisher::Event::lset, key);
        
        return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string LTrimCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int start = Utils::convertToInt(mTokens[2].first);
        int stop = Utils::convertToInt(mTokens[3].first);
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return redis_const::NULL_BULK_STRING;
        }
        
        listMap.trim(key, start, stop);
        
        if (!listMap.keyExists(key)) {
            FIRE_EVENT(EventPublisher::Event::del, key);
        }
        FIRE_EVENT(EventPublisher::Event::ltrim, key);
        
        return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string LInsertCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string pos = mTokens[2].first;
        if (pos != "BEFORE" && pos != "AFTER") {
            throw invalid_argument("Invalid args");
        }
        string pivot = mTokens[3].first;
        string val = mTokens[4].first;
        
        ListMap& listMap = data.getListMap();
        
        if (!listMap.keyExists(key)) {
            return redis_const::NULL_BULK_STRING;
        }
        
        int len = listMap.insert(key, pos, pivot, val);
        
        FIRE_EVENT(EventPublisher::Event::linsert, key);
        
        return RedisProtocol::serializeNonArray(to_string(len), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string BLCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        int timeout = Utils::convertToInt(mTokens[mTokens.size() - 1].first);
        string res;
        list<string> watchKeys;
        
        ListMap& listMap = data.getListMap();

        if (timeout != 0 && (mTimestamp + timeout < time(0))) {
            return RedisProtocol::serializeArray({
                make_pair(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::BULK_STRING)
            });
        }
        
        for (int i = 1; i < mTokens.size() - 1; ++i) {
            const std::string& key = mTokens[i].first;
            if (listMap.keyExists(key) && listMap.len(key) != 0) {
                
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
                    make_pair(key, RedisProtocol::DataType::BULK_STRING),
                    make_pair(res, RedisProtocol::DataType::BULK_STRING)
                };
                return RedisProtocol::serializeArray(arr);
            }
            watchKeys.push_back(key);
        }
        
        ctx.setPendingtBLCommand(std::shared_ptr<Command>(this->clone()),
                                 timeout,
                                 watchKeys);
        
        return "";
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string BRPopLPushCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        const std::string& source = mTokens[1].first;
        const std::string& dest = mTokens[2].first;
        int timeout = Utils::convertToInt(mTokens[3].first);
        list<string> watchKeys;
        
        ListMap& listMap = data.getListMap();
        
        if (timeout != 0 && (mTimestamp + timeout < time(0))) {
            return RedisProtocol::serializeArray({
                make_pair(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::BULK_STRING)
            });
        }
        
        if (listMap.keyExists(source) && listMap.len(source) != 0) {
            // Found a non-empty list. Pop it up and return.
            string val = listMap.pop(source, ListMap::Position::BACK);
            if (!listMap.keyExists(source)) {
                FIRE_EVENT(EventPublisher::Event::del, source);
            }
            FIRE_EVENT(EventPublisher::Event::rpop, source);
            
            listMap.push(dest, val, ListMap::Position::FRONT);
            FIRE_EVENT(EventPublisher::Event::lpush, dest);
            
            return RedisProtocol::serializeNonArray(val, RedisProtocol::DataType::BULK_STRING);
        }

        watchKeys.push_back(source);
        
        ctx.setPendingtBLCommand(std::shared_ptr<Command>(this->clone()),
                                 timeout,
                                 watchKeys);
        
        return "";
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}
