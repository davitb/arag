#include <sstream>
#include "Commands.h"
#include "RedisProtocol.h"

using namespace std;
using namespace cache_server;

shared_ptr<Command> Command::createCommand(string cmdline)
{
    vector<string> tokens = RedisProtocol::parse(cmdline);
    Command* pCmd = nullptr;
    
    if (tokens.size() == 0) {
        throw invalid_argument("Invalid Command");
    }
    
    if (tokens[0] == "SET") {
        pCmd = new SetCommand();
    }
    else
    if (tokens[0] == "GET") {
        pCmd = new GetCommand();
    }
    else
    if (tokens[0] == "GETSET") {
        pCmd = new GetSetCommand();
    }
    else
    if (tokens[0] == "APPEND") {
        pCmd = new AppendCommand();
    }
    else
    if (tokens[0] == "INCR") {
        pCmd = new IncrCommand();
    }
    else
    if (tokens[0] == "GETRANGE") {
        pCmd = new GetRangeCommand();
    }
    else
    if (tokens[0] == "MGET") {
        pCmd = new MGetCommand();
    }
    else
    if (tokens[0].substr(0, string(CMD_INTERNAL_PREFIX).length()) == CMD_INTERNAL_PREFIX) {
        pCmd = new InternalCommand();
    }
    else {
        throw invalid_argument("Invalid Command");
    }
    
    pCmd->setTokens(tokens);
    return shared_ptr<Command>(pCmd);
}

//-------------------------------------------------------------------------

void Command::setTokens(const std::vector<std::string> &tokens)
{
    mTokens = tokens;
}

string Command::getCommandName() const
{
    if (mTokens.size() == 0) {
        return "";
    }
    
    return mTokens[0];
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

        string key = mTokens[1];
        string val = mTokens[2];
        string expType = "";
        string expVal = "";
        int intVal = 0;

        if (cmdNum > Consts::MIN_ARG_NUM) {
            if (cmdNum != Consts::MAX_ARG_NUM) {
                throw invalid_argument("Invalid args");
            }
            expType = mTokens[3];
            expVal = mTokens[4];

            if (expType != "EX") {
                throw invalid_argument("Invalid args");
            }

            intVal = RedisProtocol::convertToInt(expVal);
        }

        map.set(key, val, intVal);

        return RedisProtocol::serializeNonArray("OK",
                                        RedisProtocol::DataType::SIMPLE_STRING);
    }
    catch (std::exception& e) {
        return cache_server::NULL_BULK_STRING;
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
        
        return RedisProtocol::serializeNonArray(map.get(mTokens[1]),
                                                RedisProtocol::DataType::BULK_STRING);
    }
    catch (std::exception& e) {
        return cache_server::NULL_BULK_STRING;
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
        
        string key = mTokens[1];
        string val = mTokens[2];
        string expType = "";
        string expVal = "";
        int intVal = 0;
        
        if (cmdNum > Consts::MIN_ARG_NUM) {
            if (cmdNum != Consts::MAX_ARG_NUM) {
                throw invalid_argument("Invalid args");
            }
            
            expType = mTokens[3];
            expVal = mTokens[4];

            if (expType != "EX") {
                throw invalid_argument("Invalid args");
            }
            
            intVal = RedisProtocol::convertToInt(expVal);
        }
        
        return RedisProtocol::serializeNonArray(map.getset(key, val, intVal),
                                                RedisProtocol::DataType::BULK_STRING);
    }
    catch (std::exception& e) {
        return cache_server::NULL_BULK_STRING;
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
        
        string key = mTokens[1];
        string val = mTokens[2];
        string expType = "";
        string expVal = "";
        int intVal = 0;
        
        if (cmdNum > Consts::MIN_ARG_NUM) {
            if (cmdNum != Consts::MAX_ARG_NUM) {
                throw invalid_argument("Invalid args");
            }
            
            expType = mTokens[3];
            expVal = mTokens[4];

            if (expType != "EX") {
                throw invalid_argument("Invalid args");
            }
            
            intVal = RedisProtocol::convertToInt(expVal);
        }
        
        return RedisProtocol::serializeNonArray(to_string(map.append(key, val, intVal)),
                                                RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return cache_server::NULL_BULK_STRING;
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
        
        string key = mTokens[1];
        
        return RedisProtocol::serializeNonArray(to_string(map.incr(key)),
                                                RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return cache_server::NULL_BULK_STRING;
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
        
        string key = mTokens[1];
        string start = mTokens[2];
        string end = mTokens[3];
        
        return RedisProtocol::serializeNonArray(map.getRange(key,
                                                             RedisProtocol::convertToInt(start),
                                                             RedisProtocol::convertToInt(end)),
                                                RedisProtocol::DataType::BULK_STRING);
    }
    catch (std::exception& e) {
        return cache_server::NULL_BULK_STRING;
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

        vector<string> keys(mTokens.begin() + 1, mTokens.end());

        return RedisProtocol::serializeArray(map.mget(keys));
    }
    catch (std::exception& e) {
        return cache_server::NULL_BULK_STRING;
    }
}
