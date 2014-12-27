#include <sstream>
#include "Commands.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace cache_server;
using namespace cache_server::command_const;

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
    if (tokens[0].first == "MGET") {
        pCmd = new MGetCommand();
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
        
        return RedisProtocol::serializeNonArray(to_string(map.incr(key)),
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

//----------------------------------------------------------------------------
InternalCommand::InternalCommand(std::string name)
{
    mTokens.push_back(std::make_pair(name, 0));
}
