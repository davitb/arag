#include <sstream>
#include <climits>
#include <algorithm>
#include "StringCmds.h"
#include "HashCmds.h"
#include "ListCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

static Command* getCommandByName(const string& cmdName)
{
    static unordered_map<string, shared_ptr<Command>> sNameToCommand;
    
    if (sNameToCommand.empty()) {
        // Internal Commands
        sNameToCommand[CMD_INTERNAL_STOP] = shared_ptr<Command>(new InternalCommand(CMD_INTERNAL_STOP));
        sNameToCommand[CMD_INTERNAL_CLEANUP] = shared_ptr<Command>(new InternalCommand(CMD_INTERNAL_CLEANUP));
        
        // String Commands
        sNameToCommand["SET"] = shared_ptr<Command>(new SetCommand());
        sNameToCommand["GET"] = shared_ptr<Command>(new GetCommand());
        sNameToCommand["GETSET"] = shared_ptr<Command>(new GetSetCommand());
        sNameToCommand["APPEND"] = shared_ptr<Command>(new AppendCommand());
        sNameToCommand["INCR"] = shared_ptr<Command>(new IncrCommand());
        sNameToCommand["GETRANGE"] = shared_ptr<Command>(new GetRangeCommand());
        sNameToCommand["SETRANGE"] = shared_ptr<Command>(new SetRangeCommand());
        sNameToCommand["MGET"] = shared_ptr<Command>(new MGetCommand());
        sNameToCommand["MSET"] = shared_ptr<Command>(new MSetCommand(false));
        sNameToCommand["MSETNX"] = shared_ptr<Command>(new MSetCommand(true));
        sNameToCommand["BITCOUNT"] = shared_ptr<Command>(new BitCountCommand());
        sNameToCommand["BITOP"] = shared_ptr<Command>(new BitOpCommand());
        sNameToCommand["BITPOS"] = shared_ptr<Command>(new BitPosCommand());
        sNameToCommand["GETBIT"] = shared_ptr<Command>(new GetBitCommand());
        sNameToCommand["SETBIT"] = shared_ptr<Command>(new SetBitCommand());
        sNameToCommand["STRLEN"] = shared_ptr<Command>(new StrlenCommand());
        sNameToCommand["INCRBY"] = shared_ptr<Command>(new IncrByCommand());
        sNameToCommand["INCRBYFLOAT"] = shared_ptr<Command>(new IncrByFloatCommand());
        sNameToCommand["DECR"] = shared_ptr<Command>(new DecrCommand());
        sNameToCommand["DECRBY"] = shared_ptr<Command>(new DecrByCommand());
        
        // Hash Commands
        sNameToCommand["HSET"] = shared_ptr<Command>(new HSetCommand(HSetCommand::CmdType::SET));
        sNameToCommand["HSETNX"] = shared_ptr<Command>(new HSetCommand(HSetCommand::CmdType::SETNX));
        sNameToCommand["HMSET"] = shared_ptr<Command>(new HSetCommand(HSetCommand::CmdType::MSET));
        sNameToCommand["HGET"] = shared_ptr<Command>(new HGetCommand());
        sNameToCommand["HEXISTS"] = shared_ptr<Command>(new HExistsCommand());
        sNameToCommand["HDEL"] = shared_ptr<Command>(new HDelCommand());
        sNameToCommand["HGETALL"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::CmdType::GETALL));
        sNameToCommand["HKEYS"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::CmdType::KEYS));
        sNameToCommand["HVALS"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::CmdType::VALS));
        sNameToCommand["HMGET"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::CmdType::MGET));
        sNameToCommand["HLEN"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::CmdType::LEN));
        sNameToCommand["HINCRBY"] = shared_ptr<Command>(new HIncrByCommand(HIncrByCommand::CmdType::INCRBY));
        sNameToCommand["HINCRBYFLOAT"] =
            shared_ptr<Command>(new HIncrByCommand(HIncrByCommand::CmdType::INCRBYFLOAT));
        
        // List Commands
        sNameToCommand["RPUSH"] = shared_ptr<Command>(new LPushCommand(LPushCommand::CmdType::RPUSH));
        sNameToCommand["RPUSHX"] = shared_ptr<Command>(new LPushCommand(LPushCommand::CmdType::RPUSHX));
        sNameToCommand["LPUSH"] = shared_ptr<Command>(new LPushCommand(LPushCommand::CmdType::LPUSH));
        sNameToCommand["LPUSHX"] = shared_ptr<Command>(new LPushCommand(LPushCommand::CmdType::LPUSHX));
        sNameToCommand["LLEN"] = shared_ptr<Command>(new LGetCommand(LGetCommand::CmdType::LEN));
        sNameToCommand["LINDEX"] = shared_ptr<Command>(new LGetCommand(LGetCommand::CmdType::INDEX));
        sNameToCommand["LPOP"] = shared_ptr<Command>(new LRemCommand(LRemCommand::CmdType::LPOP));
        sNameToCommand["RPOP"] = shared_ptr<Command>(new LRemCommand(LRemCommand::CmdType::RPOP));
        sNameToCommand["LREM"] = shared_ptr<Command>(new LRemCommand(LRemCommand::CmdType::REM));
        sNameToCommand["RPOPLPUSH"] = shared_ptr<Command>(new LRemCommand(LRemCommand::CmdType::RPOPLPUSH));
        sNameToCommand["LRANGE"] = shared_ptr<Command>(new LRangeCommand());
        sNameToCommand["LSET"] = shared_ptr<Command>(new LSetCommand());
        sNameToCommand["LTRIM"] = shared_ptr<Command>(new LTrimCommand());
        sNameToCommand["LINSERT"] = shared_ptr<Command>(new LInsertCommand());
    }
    
    if (sNameToCommand.count(cmdName) == 0) {
        throw invalid_argument("Invalid command");
    }
    
    return sNameToCommand[cmdName].get();
}

Command& Command::getCommand(const string& cmdline)
{
    // Skip parsing if this is an internal command
    if (cmdline.substr(0, CMD_INTERNAL_PREFIX.length()) == CMD_INTERNAL_PREFIX) {
        return *getCommandByName(cmdline);
    }
    
    vector<pair<string, int>> tokens = RedisProtocol::parse(cmdline);
    
    if (tokens.size() == 0) {
        throw invalid_argument("Invalid Command");
    }
    
    Command* pCmd = getCommandByName(tokens[0].first);
    if (pCmd == nullptr) {
        throw invalid_argument("Invalid Command");
    }
    
    pCmd->setTokens(tokens);
    
    return *pCmd;
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

