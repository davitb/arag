#include <sstream>
#include <climits>
#include <algorithm>
#include "StringCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

CommandResultPtr SetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        string key = mTokens[1].first;
        string val = mTokens[2].first;
        
        // Extract expiration time
        int exp = 0;
        StringMap::ExpirationType expType = StringMap::ExpirationType::SEC;
        extractExpirationNum(mTokens, Consts::MIN_ARG_NUM, Consts::MAX_ARG_NUM - 1, &expType, &exp);
        
        StringMap::SetKeyPolicy policy = StringMap::SetKeyPolicy::CREATE_IF_DOESNT_EXIST;
        // Extract NX/XX if it's provided
        if (cmdNum == Consts::MAX_ARG_NUM) {
            policy = StringMap::SetKeyPolicy::ONLY_IF_DOESNT_ALREADY_EXISTS;
            if (mTokens[4].first == "XX") {
                policy = StringMap::SetKeyPolicy::ONLY_IF_ALREADY_EXISTS;
            }
        }
        
        map.set(key, val, expType, exp, policy);
        
        FIRE_EVENT(EventPublisher::set, key);
        
        return CommandResult::redisOKResult();
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr GetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        return CommandResultPtr(new CommandResult(map.get(mTokens[1].first),
                                                RedisProtocol::BULK_STRING));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr GetSetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();

    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string val = mTokens[2].first;
        
        return CommandResultPtr(new CommandResult(map.getset(key, val),
                                                RedisProtocol::BULK_STRING));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr AppendCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string val = mTokens[2].first;
        int res = map.append(key, val);

        FIRE_EVENT(EventPublisher::Event::append, key);
        
        return CommandResultPtr(new CommandResult(to_string(res),
                                                RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr IncrCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
    
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        string resp = to_string(map.incrBy(key, 1));
        
        FIRE_EVENT(EventPublisher::Event::incrby, key);

        return CommandResultPtr(new CommandResult(resp, RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr IncrByCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int by = Utils::convertToInt(mTokens[2].first);
        
        string resp = to_string(map.incrBy(key, by));
        
        FIRE_EVENT(EventPublisher::Event::incrby, key);
        
        return CommandResultPtr(new CommandResult(resp, RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr IncrByFloatCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        double by = Utils::convertToDouble(mTokens[2].first);
        
        const string& resp = map.incrBy(key, by);
        
        FIRE_EVENT(EventPublisher::Event::incrbyfloat, key);
        
        return CommandResultPtr(new CommandResult(resp, RedisProtocol::BULK_STRING));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr DecrCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        string resp = to_string(map.incrBy(key, -1));
        
        FIRE_EVENT(EventPublisher::Event::incrbyfloat, key);
        
        return CommandResultPtr(new CommandResult(resp, RedisProtocol::INTEGER));;
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr DecrByCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int by = Utils::convertToInt(mTokens[2].first);
        
        int res = map.incrBy(key, by * -1);
        
        FIRE_EVENT(EventPublisher::Event::incrby, key);
        
        return CommandResultPtr(new CommandResult(to_string(res),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr GetRangeCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string start = mTokens[2].first;
        string end = mTokens[3].first;
        
        return CommandResultPtr(new CommandResult(map.getRange(key,
                                          Utils::convertToInt(start),
                                          Utils::convertToInt(end)),
                                          RedisProtocol::BULK_STRING));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SetRangeCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int offset = Utils::convertToInt(mTokens[2].first);
        if (offset < 0) {
            throw invalid_argument("Invalid args");
        }
        
        string value = mTokens[3].first;
        int valueLen = (int)value.length();
        
        string oldVal = "";
        size_t finalLen = offset + valueLen;
        
        try {
            oldVal = map.get(key);
            
            int oldValLen = (int)oldVal.length();
            
            finalLen = max(oldValLen, offset + valueLen);
            
            if (oldValLen < offset) {
                for (size_t i = 0; i < offset - oldValLen; ++i) {
                    oldVal += '\x00';
                }
            }
        }
        catch (...) {
            oldVal.reserve(offset + valueLen);
            for (size_t i = 0; i < offset + valueLen; ++i) {
                oldVal += '\x00';
            }
        }
        
        for (size_t i = 0; i < valueLen; ++i) {
            oldVal[offset + i] = value[i];
        }
        
        map.set(key, oldVal);
        
        FIRE_EVENT(EventPublisher::setrange, key);
        
        return CommandResultPtr(new CommandResult(to_string(finalLen),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr MGetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        vector<string> keys(mTokens.size() - 1);
        
        for (int i = 0; i < mTokens.size() - 1; ++i) {
            keys[i] = mTokens[i + 1].first;
        }
        
        return CommandResultPtr(new CommandResult(map.mget(keys)));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr MSetCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        size_t size = mTokens.size();
        
        if (size % 2 != 1) {
            throw invalid_argument("Invalid args");
        }

        // If this is a MSETNX command - make sure that all given keys exist
        if (mNX) {
            for (int i = 1; i < size; i += 2) {
                try {
                    map.get(mTokens[i].first);
                    // There is a key that does exist - fail
                    return CommandResultPtr(new CommandResult("0",
                                                              RedisProtocol::INTEGER));
                }
                catch (...) {
                }
            }
        }
        
        for (int i = 1; i < size; i += 2) {
            map.set(mTokens[i].first, mTokens[i + 1].first);
            FIRE_EVENT(EventPublisher::set, mTokens[i].first);
        }
        
        // Need to return different values and types depending on command type (MSET or MSETNX)
        if (mNX) {
            return CommandResultPtr(new CommandResult("1",
                                                      RedisProtocol::INTEGER));
        }

        return CommandResult::redisOKResult();
    }
    catch (std::exception& e) {
    }
    
    return CommandResultPtr(new CommandResult(redis_const::INVALID_ARGUMENT,
                                              RedisProtocol::ERROR));
}

//-------------------------------------------------------------------------

CommandResultPtr BitCountCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int start = 0;
        int end = INT_MAX;

        if (cmdNum != Consts::MIN_ARG_NUM) {
            if (cmdNum != Consts::MAX_ARG_NUM) {
                throw invalid_argument("Invalid args");
            }
            
            start = Utils::convertToInt(mTokens[2].first);
            end = Utils::convertToInt(mTokens[3].first);
        }

        string str = map.getRange(key, start, end);
        
        int total = 0;
        for (char ch : str) {
            total += Utils::countSetBits(ch);
        }
        
        return CommandResultPtr(new CommandResult(to_string(total),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr BitOpCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string op = mTokens[1].first;
        string destKey = mTokens[2].first;
        vector<string> keys(mTokens.size() - 3);
        
        for (int i = 3; i < mTokens.size(); ++i) {
            keys[i - 3] = mTokens[i].first;
        }
        
        vector<pair<string, int>> vals = map.mget(keys);
        
        string val = Utils::performBitOperation(op, vals);
        map.set(destKey, val);
        
        return CommandResultPtr(new CommandResult(to_string(val.length()),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr BitPosCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int bit = Utils::convertToInt(mTokens[2].first);
        if (bit != 0 && bit != 1) {
            throw invalid_argument("Invalid args");
        }

        int start = 0;
        int end = INT_MAX;
        
        if (cmdNum != Consts::MIN_ARG_NUM) {
            
            start = Utils::convertToInt(mTokens[3].first);
            
            if (cmdNum == Consts::MAX_ARG_NUM) {
                end = Utils::convertToInt(mTokens[4].first);
            }
        }
        
        string val = map.getRange(key, start, end);
        
        int pos = Utils::getBitPos(val, bit, (cmdNum > Consts::MIN_ARG_NUM));
        
        // Note that bit positions are returned always as absolute values starting
        // from bit zero even when start and end are used to specify a range.
        if (pos != -1) {
            pos += (start * 8);
        }
        
        return CommandResultPtr(new CommandResult(to_string(pos),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr GetBitCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int offset = Utils::convertToInt(mTokens[2].first);
        if (offset < 0 || offset >= SetBitCommand::Consts::MAX_OFFSET) {
            throw invalid_argument("Invalid args");
        }

        string val = "";
        try {
            val = map.get(key);
        }
        catch (...) {}

        int pos = Utils::getBit(val, offset);
        
        return CommandResultPtr(new CommandResult(to_string(pos),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SetBitCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        int originalBit = 0;
        string key = mTokens[1].first;
        int offset = Utils::convertToInt(mTokens[2].first);
        if (offset < 0 || offset >= Consts::MAX_OFFSET) {
            throw invalid_argument("Invalid args");
        }
        int bitValue = Utils::convertToInt(mTokens[3].first);
        if (bitValue != 0 && bitValue != 1) {
            throw invalid_argument("Invalid args");
        }
        
        string val = "";
        try {
            val = map.get(key);
            originalBit = Utils::getBit(val, offset);
        }
        catch (...) {
            int bytesNum = offset / 8 + 1;
            val.reserve(bytesNum);
            for (int i = 0; i < bytesNum; ++i) {
                val += '\x00';
            }
            originalBit = 0;
        }
        
        Utils::setBit(val, offset, bitValue);
        
        map.set(key, val);
        
        return CommandResultPtr(new CommandResult(to_string(originalBit),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr StrlenCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    StringMap& map = data.getStringMap();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        size_t len = 0;
        
        string val = "";
        try {
            len = map.get(key).length();
        }
        catch (...) {
            len = 0;
        }
        
        return CommandResultPtr(new CommandResult(to_string(len),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}
