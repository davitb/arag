#include <sstream>
#include <climits>
#include <algorithm>
#include "StringCmds.h"
#include "HashCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

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
    
    // String commands
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
    pCmd = new MSetCommand(false);
    }
    else
    if (tokens[0].first == "MSETNX") {
    pCmd = new MSetCommand(true);
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
    if (tokens[0].first == "INCRBYFLOAT") {
    pCmd = new IncrByFloatCommand();
    }
    else
    if (tokens[0].first == "DECR") {
    pCmd = new DecrCommand();
    }
    else
    if (tokens[0].first == "DECRBY") {
        pCmd = new DecrByCommand();
    }

    // Hash Commands
    else
    if (tokens[0].first == "HSET") {
        pCmd = new HSetCommand(HSetCommand::CmdType::SET);
    }
    else
    if (tokens[0].first == "HSETNX") {
        pCmd = new HSetCommand(HSetCommand::CmdType::SETNX);
    }
    else
    if (tokens[0].first == "HMSET") {
        pCmd = new HSetCommand(HSetCommand::CmdType::MSET);
    }
    else
    if (tokens[0].first == "HGET") {
        pCmd = new HGetCommand();
    }
    else
    if (tokens[0].first == "HEXISTS") {
        pCmd = new HExistsCommand();
    }
    else
    if (tokens[0].first == "HDEL") {
        pCmd = new HDelCommand();
    }
    else
    if (tokens[0].first == "HGETALL") {
        pCmd = new HGetAllCommand(HGetAllCommand::CmdType::GETALL);
    }
    else
    if (tokens[0].first == "HKEYS") {
        pCmd = new HGetAllCommand(HGetAllCommand::CmdType::KEYS);
    }
    else
    if (tokens[0].first == "HVALS") {
        pCmd = new HGetAllCommand(HGetAllCommand::CmdType::VALS);
    }
    else
    if (tokens[0].first == "HMGET") {
        pCmd = new HGetAllCommand(HGetAllCommand::CmdType::MGET);
    }
    else
    if (tokens[0].first == "HLEN") {
        pCmd = new HGetAllCommand(HGetAllCommand::CmdType::LEN);
    }
    else
    if (tokens[0].first == "HINCRBY") {
        pCmd = new HIncrByCommand(HIncrByCommand::CmdType::INCRBY);
    }
    else
    if (tokens[0].first == "HINCRBYFLOAT") {
        pCmd = new HIncrByCommand(HIncrByCommand::CmdType::INCRBYFLOAT);
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

void Command::extractExpirationNum(const vector<pair<string, int>>& tokens,
                          int minArgsNum,
                          int maxArgsNum,
                          StringMap::ExpirationType* pExpType,
                          int* pExp)
{
    size_t cmdNum = tokens.size();
    
    if (cmdNum != minArgsNum) {
        
        if (cmdNum != maxArgsNum || (pExp == nullptr || pExpType == nullptr)) {
            throw invalid_argument("Invalid args");
        }
        
        string expType = tokens[3].first;
        string expVal = tokens[4].first;
        
        if (expType != "EX" && expType != "PX") {
            throw invalid_argument("Invalid args");
        }
        
        *pExpType = expType == "EX" ? StringMap::ExpirationType::SEC : StringMap::ExpirationType::MSEC;
        *pExp = Utils::convertToInt(expVal);
    }
}

//----------------------------------------------------------------------------

InternalCommand::InternalCommand(std::string name)
{
    mTokens.push_back(std::make_pair(name, 0));
}

