#include <iostream>
#include <sstream>
#include "Commands.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <climits>
#include <algorithm>

using namespace std;
using namespace arag;
using namespace arag::command_const;

int extractExpirationNum(const vector<pair<string, int>>& tokens, int minArgsNum, int maxArgsNum)
{
    size_t cmdNum = tokens.size();
    
    if (cmdNum != minArgsNum) {
    
        if (cmdNum != maxArgsNum) {
            throw invalid_argument("Invalid args");
        }
        
        string expType = tokens[3].first;
        string expVal = tokens[4].first;
        
        if (expType != "EX") {
            throw invalid_argument("Invalid args");
        }
        
        return Utils::convertToInt(expVal);
    }
    
    return 0;
}

shared_ptr<Command> Command::createCommand(string cmdline)
{
    if (cmdline.substr(0, CMD_INTERNAL_PREFIX.length()) == CMD_INTERNAL_PREFIX) {
        return shared_ptr<Command>(new InternalCommand(cmdline));
    }
    
    vector<pair<string, int>> tokens = RedisProtocol::parse(cmdline);
    Command* pCmd = nullptr;
    
    if (tokens.size() == 0) {
        throw invalid_argument("Invalid Command");
    }
    
    if (tokens[0].first == "SET") {
        pCmd = new SetCommand();
    }
    else
    if (tokens[0].first == "GET") {
        pCmd = new GetCommand();
    }
    else
    if (tokens[0].first == "GETSET") {
        pCmd = new GetSetCommand();
    }
    else
    if (tokens[0].first == "APPEND") {
        pCmd = new AppendCommand();
    }
    else
    if (tokens[0].first == "INCR") {
        pCmd = new IncrCommand();
    }
    else
    if (tokens[0].first == "GETRANGE") {
        pCmd = new GetRangeCommand();
    }
    else
    if (tokens[0].first == "SETRANGE") {
        pCmd = new SetRangeCommand();
    }
    else
    if (tokens[0].first == "MGET") {
        pCmd = new MGetCommand();
    }
    else
    if (tokens[0].first == "MSET") {
        pCmd = new MSetCommand();
    }
    else
    if (tokens[0].first == "BITCOUNT") {
        pCmd = new BitCountCommand();
    }
    else
    if (tokens[0].first == "BITOP") {
        pCmd = new BitOpCommand();
    }
    else
    if (tokens[0].first == "BITPOS") {
        pCmd = new BitPosCommand();
    }
    else
    if (tokens[0].first == "GETBIT") {
        pCmd = new GetBitCommand();
    }
    else
    if (tokens[0].first == "SETBIT") {
        pCmd = new SetBitCommand();
    }
    else
    if (tokens[0].first == "STRLEN") {
        pCmd = new StrlenCommand();
    }
    else
    if (tokens[0].first == "INCRBY") {
        pCmd = new IncrByCommand();
    }
    else
    if (tokens[0].first == "DECR") {
        pCmd = new DecrCommand();
    }
    else
    if (tokens[0].first == "DECRBY") {
        pCmd = new DecrByCommand();
    }
    else {
        throw invalid_argument("Invalid Command");
    }
    
    pCmd->setTokens(tokens);
    return shared_ptr<Command>(pCmd);
}

//-------------------------------------------------------------------------

void Command::setTokens(const vector<pair<string, int>> &tokens)
{
    mTokens = tokens;
}

string Command::getCommandName() const
{
    if (mTokens.size() == 0) {
        return "";
    }
    
    return mTokens[0].first;
}


//----------------------------------------------------------------------------

InternalCommand::InternalCommand(std::string name)
{
    mTokens.push_back(std::make_pair(name, 0));
}

//-------------------------------------------------------------------------

string SetCommand::execute(CSMap& map)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        string key = mTokens[1].first;
        string val = mTokens[2].first;
        int intVal = extractExpirationNum(mTokens, Consts::MIN_ARG_NUM, Consts::MAX_ARG_NUM);
        
        map.set(key, val, intVal);

        return RedisProtocol::serializeNonArray("OK",
                                        RedisProtocol::DataType::SIMPLE_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string GetCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        return RedisProtocol::serializeNonArray(map.get(mTokens[1].first),
                                                RedisProtocol::DataType::BULK_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string GetSetCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();

    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string val = mTokens[2].first;
        int intVal = extractExpirationNum(mTokens, Consts::MIN_ARG_NUM, Consts::MAX_ARG_NUM);
        
        return RedisProtocol::serializeNonArray(map.getset(key, val, intVal),
                                                RedisProtocol::DataType::BULK_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string AppendCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();

    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string val = mTokens[2].first;
        int intVal = extractExpirationNum(mTokens, Consts::MIN_ARG_NUM, Consts::MAX_ARG_NUM);
        
        return RedisProtocol::serializeNonArray(to_string(map.append(key, val, intVal)),
                                                RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string IncrCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    try {
    
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        return RedisProtocol::serializeNonArray(to_string(map.incrBy(key, 1)),
                                                RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string IncrByCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    try {
        
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int by = Utils::convertToInt(mTokens[2].first);
        
        return RedisProtocol::serializeNonArray(to_string(map.incrBy(key, by)),
                                                RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string DecrCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    try {
        
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        return RedisProtocol::serializeNonArray(to_string(map.incrBy(key, -1)),
                                                RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string DecrByCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    try {
        
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int by = Utils::convertToInt(mTokens[2].first);
        
        return RedisProtocol::serializeNonArray(to_string(map.incrBy(key, by * -1)),
                                                RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string GetRangeCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();

    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string start = mTokens[2].first;
        string end = mTokens[3].first;
        
        return RedisProtocol::serializeNonArray(map.getRange(key,
                                                             Utils::convertToInt(start),
                                                             Utils::convertToInt(end)),
                                                RedisProtocol::DataType::BULK_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string SetRangeCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
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
        
        return RedisProtocol::serializeNonArray(to_string(finalLen), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string MGetCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();

    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        vector<string> keys(mTokens.size() - 1);
        
        for (int i = 0; i < mTokens.size() - 1; ++i) {
            keys[i] = mTokens[i + 1].first;
        }
        
        return RedisProtocol::serializeArray(map.mget(keys));
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string MSetCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        size_t size = mTokens.size();
        
        if (size % 2 != 1) {
            throw invalid_argument("Invalid args");
        }
        
        for (int i = 1; i < size; i += 2) {
            map.set(mTokens[i].first, mTokens[i + 1].first);
        }
        
        return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string BitCountCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
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
        
        return RedisProtocol::serializeNonArray(to_string(total), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string BitOpCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
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
        
        return RedisProtocol::serializeNonArray(to_string(val.length()), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string BitPosCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
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
        
        return RedisProtocol::serializeNonArray(to_string(pos), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string GetBitCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
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
        
        return RedisProtocol::serializeNonArray(to_string(pos), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string SetBitCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
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
            std:cout << "val.length: " << val.length() << endl;
            originalBit = 0;
        }
        
        Utils::setBit(val, offset, bitValue);
        
        map.set(key, val);
        
        return RedisProtocol::serializeNonArray(to_string(originalBit), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string StrlenCommand::execute(CSMap& map)
{
    size_t cmdNum = mTokens.size();
    
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
        
        return RedisProtocol::serializeNonArray(to_string(len), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}
