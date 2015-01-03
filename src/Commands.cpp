#include <sstream>
#include <climits>
#include <algorithm>
#include "ConnectionCmds.h"
#include "StringCmds.h"
#include "HashCmds.h"
#include "ListCmds.h"
#include "SetCmds.h"
#include "SortedSetCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <iostream>

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

        // Connection Commands
        sNameToCommand["PING"] = shared_ptr<Command>(new PingCommand(PingCommand::CmdType::PING));
        sNameToCommand["ECHO"] = shared_ptr<Command>(new PingCommand(PingCommand::CmdType::ECHO));
        
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
        
        // Set Commands
        sNameToCommand["SADD"] = shared_ptr<Command>(new SAddCommand());
        sNameToCommand["SMEMBERS"] = shared_ptr<Command>(new SMembersCommand());
        sNameToCommand["SCARD"] = shared_ptr<Command>(new SCardCommand(SCardCommand::CmdType::CARD));
        sNameToCommand["SISMEMBER"] = shared_ptr<Command>(new SCardCommand(SCardCommand::CmdType::ISMEMBER));
        sNameToCommand["SPOP"] = shared_ptr<Command>(new SRemCommand(SRemCommand::CmdType::POP));
        sNameToCommand["SREM"] = shared_ptr<Command>(new SRemCommand(SRemCommand::CmdType::REM));
        sNameToCommand["SDIFF"] = shared_ptr<Command>(new SDiffCommand(SDiffCommand::CmdType::DIFF));
        sNameToCommand["SDIFFSTORE"] = shared_ptr<Command>(new SDiffCommand(SDiffCommand::CmdType::DIFFSTORE));
        sNameToCommand["SINTER"] = shared_ptr<Command>(new SInterCommand(SInterCommand::CmdType::INTER));
        sNameToCommand["SINTERSTORE"] = shared_ptr<Command>(new SInterCommand(SInterCommand::CmdType::INTERSTORE));
        sNameToCommand["SUNION"] = shared_ptr<Command>(new SUnionCommand(SUnionCommand::CmdType::UNION));
        sNameToCommand["SUNIONSTORE"] = shared_ptr<Command>(new SUnionCommand(SUnionCommand::CmdType::UNIONSTORE));
        sNameToCommand["SMOVE"] = shared_ptr<Command>(new SMoveCommand());
        sNameToCommand["SRANDMEMBER"] = shared_ptr<Command>(new SRandMemberCommand());
        
        // Sorted Set Commands
        sNameToCommand["ZADD"] = shared_ptr<Command>(new ZAddCommand());
        sNameToCommand["ZRANGE"] = shared_ptr<Command>(new ZRangeCommand(ZRangeCommand::CmdType::RANGE));
        sNameToCommand["ZREVRANGE"] = shared_ptr<Command>(new ZRangeCommand(ZRangeCommand::CmdType::REVRANGE));
        sNameToCommand["ZINCRBY"] = shared_ptr<Command>(new ZIncrByCommand());
        sNameToCommand["ZSCORE"] = shared_ptr<Command>(new ZScoreCommand(ZScoreCommand::CmdType::SCORE));
        sNameToCommand["ZRANK"] = shared_ptr<Command>(new ZScoreCommand(ZScoreCommand::CmdType::RANK));
        sNameToCommand["ZREVRANK"] = shared_ptr<Command>(new ZScoreCommand(ZScoreCommand::CmdType::REVRANK));
        sNameToCommand["ZCOUNT"] = shared_ptr<Command>(new ZCountCommand(ZCountCommand::CmdType::COUNT));
        sNameToCommand["ZLEXCOUNT"] = shared_ptr<Command>(new ZCountCommand(ZCountCommand::CmdType::LEXCOUNT));
        sNameToCommand["ZCARD"] = shared_ptr<Command>(new ZCountCommand(ZCountCommand::CmdType::CARD));
        sNameToCommand["ZREM"] = shared_ptr<Command>(new ZRemCommand(ZRemCommand::CmdType::REM));
        sNameToCommand["ZUNIONSTORE"] = shared_ptr<Command>(new ZUnionCommand(ZUnionCommand::CmdType::UNION));
        sNameToCommand["ZINTERSTORE"] = shared_ptr<Command>(new ZUnionCommand(ZUnionCommand::CmdType::INTERSECT));
        sNameToCommand["ZRANGEBYSCORE"] = shared_ptr<Command>(new ZRangeByCommand(ZRangeByCommand::CmdType::RANGEBYSCORE));
        sNameToCommand["ZREVRANGEBYSCORE"] = shared_ptr<Command>(new ZRangeByCommand(ZRangeByCommand::CmdType::REVRANGEBYSCORE));
        sNameToCommand["ZREMRANGEBYSCORE"] = shared_ptr<Command>(new ZRemByCommand(ZRemByCommand::CmdType::REMRANGEBYSCORE));
        sNameToCommand["ZREMRANGEBYRANK"] = shared_ptr<Command>(new ZRemByCommand(ZRemByCommand::CmdType::REMRANGEBYRANK));
        sNameToCommand["ZREMRANGEBYLEX"] = shared_ptr<Command>(new ZRemByCommand(ZRemByCommand::CmdType::REMRANGEBYLEX));
        sNameToCommand["ZRANGEBYLEX"] = shared_ptr<Command>(new ZRangeByLexCommand(ZRangeByLexCommand::CmdType::RANGEBYLEX));
        sNameToCommand["ZREVRANGEBYLEX"] = shared_ptr<Command>(new ZRangeByLexCommand(ZRangeByLexCommand::CmdType::REVRANGEBYLEX));
    }
    
    string upperCaseCmd = cmdName;
    std::transform(upperCaseCmd.begin(), upperCaseCmd.end(), upperCaseCmd.begin(), ::toupper);
    
    if (sNameToCommand.count(upperCaseCmd) == 0) {
        throw invalid_argument("Invalid command");
    }
    
    return sNameToCommand[upperCaseCmd].get();
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

