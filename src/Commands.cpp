#include <sstream>
#include <climits>
#include <algorithm>
#include "ConnectionCmds.h"
#include "ServerCmds.h"
#include "StringCmds.h"
#include "HashCmds.h"
#include "ListCmds.h"
#include "SetCmds.h"
#include "SortedSetCmds.h"
#include "KeyCmds.h"
#include "HLLCmds.h"
#include "PubSubCmds.h"
#include "TransactionCmds.h"
#include "ScriptCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "AragServer.h"
#include "KeysMap.h"
#include <iostream>
#include <regex>
#include "Database.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

/*
    This function creates a static map of all supported commands when called the first time
    and then returns a clone of a command on each subsequent call.
    This way creating a new command given its name is really fast since it's just a lookup in a map.
*/
static shared_ptr<Command> getCommandByName(const string& cmdName)
{
    static unordered_map<string, shared_ptr<Command>> sNameToCommand;
    
    if (sNameToCommand.empty()) {
        // Internal Commands
        sNameToCommand[CMD_INTERNAL_STOP] = shared_ptr<Command>(new InternalCommand(CMD_INTERNAL_STOP));
        sNameToCommand[CMD_INTERNAL_CLEANUP] = shared_ptr<Command>(new InternalCommand(CMD_INTERNAL_CLEANUP));

        // Connection Commands
        sNameToCommand["PING"] = shared_ptr<Command>(new PingCommand(PingCommand::PING));
        sNameToCommand["ECHO"] = shared_ptr<Command>(new PingCommand(PingCommand::ECHO_CMD));
        sNameToCommand["SELECT"] = shared_ptr<Command>(new SelectCommand());

        // Server Commands
        sNameToCommand["INFO"] = shared_ptr<Command>(new InfoCommand());
        sNameToCommand["FLUSHDB"] = shared_ptr<Command>(new FlushCommand(FlushCommand::FLUSHDB));
        sNameToCommand["FLUSHALL"] = shared_ptr<Command>(new FlushCommand(FlushCommand::FLUSHALL));
        sNameToCommand["CLIENT"] = shared_ptr<Command>(new ClientCommand());
        sNameToCommand["CONFIG"] = shared_ptr<Command>(new ConfigCommand());
        sNameToCommand["DBSIZE"] = shared_ptr<Command>(new SingleArgumentCommand(SingleArgumentCommand::DBSIZE));
        sNameToCommand["LASTSAVE"] = shared_ptr<Command>(new SingleArgumentCommand(SingleArgumentCommand::LASTSAVE));
        sNameToCommand["TIME"] = shared_ptr<Command>(new SingleArgumentCommand(SingleArgumentCommand::TIME));
        
        // String Commands
        sNameToCommand["SET"] = shared_ptr<Command>(new SetCommand());
        sNameToCommand["SET"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["SETNX"] = shared_ptr<Command>(new SetNXCommand());
        sNameToCommand["SETNX"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["SETEX"] = shared_ptr<Command>(new SetExCommand(SetExCommand::SETEX));
        sNameToCommand["SETEX"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["PSETEX"] = shared_ptr<Command>(new SetExCommand(SetExCommand::PSETEX));
        sNameToCommand["PSETEX"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["GET"] = shared_ptr<Command>(new GetCommand());
        sNameToCommand["GET"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["GETSET"] = shared_ptr<Command>(new GetSetCommand());
        sNameToCommand["GETSET"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["APPEND"] = shared_ptr<Command>(new AppendCommand());
        sNameToCommand["APPEND"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["INCR"] = shared_ptr<Command>(new IncrCommand());
        sNameToCommand["INCR"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["GETRANGE"] = shared_ptr<Command>(new GetRangeCommand());
        sNameToCommand["GETRANGE"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["SUBSTR"] = shared_ptr<Command>(new GetRangeCommand());
        sNameToCommand["SUBSTR"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["SETRANGE"] = shared_ptr<Command>(new SetRangeCommand());
        sNameToCommand["SETRANGE"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["MGET"] = shared_ptr<Command>(new MGetCommand());
        sNameToCommand["MGET"]->setCtx(CommandCtx({1}, IMapCommon::STRING, CommandCtx::TILL_END));
        sNameToCommand["MSET"] = shared_ptr<Command>(new MSetCommand(false));
        sNameToCommand["MSET"]->setCtx(CommandCtx({1}, IMapCommon::STRING, CommandCtx::EVERY_OTHER));
        sNameToCommand["MSETNX"] = shared_ptr<Command>(new MSetCommand(true));
        sNameToCommand["MSETNX"]->setCtx(CommandCtx({1}, IMapCommon::STRING, CommandCtx::EVERY_OTHER));
        sNameToCommand["BITCOUNT"] = shared_ptr<Command>(new BitCountCommand());
        sNameToCommand["BITCOUNT"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["BITOP"] = shared_ptr<Command>(new BitOpCommand());
        sNameToCommand["BITOP"]->setCtx(CommandCtx({2}, IMapCommon::STRING, CommandCtx::TILL_END));
        sNameToCommand["BITPOS"] = shared_ptr<Command>(new BitPosCommand());
        sNameToCommand["BITPOS"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["GETBIT"] = shared_ptr<Command>(new GetBitCommand());
        sNameToCommand["GETBIT"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["SETBIT"] = shared_ptr<Command>(new SetBitCommand());
        sNameToCommand["SETBIT"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["STRLEN"] = shared_ptr<Command>(new StrlenCommand());
        sNameToCommand["STRLEN"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["INCRBY"] = shared_ptr<Command>(new IncrByCommand());
        sNameToCommand["INCRBY"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["INCRBYFLOAT"] = shared_ptr<Command>(new IncrByFloatCommand());
        sNameToCommand["INCRBYFLOAT"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["DECR"] = shared_ptr<Command>(new DecrCommand());
        sNameToCommand["DECR"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        sNameToCommand["DECRBY"] = shared_ptr<Command>(new DecrByCommand());
        sNameToCommand["DECRBY"]->setCtx(CommandCtx({1}, IMapCommon::STRING));
        
        // Hash Commands
        sNameToCommand["HSET"] = shared_ptr<Command>(new HSetCommand(HSetCommand::SET));
        sNameToCommand["HSET"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        sNameToCommand["HSETNX"] = shared_ptr<Command>(new HSetCommand(HSetCommand::SETNX));
        sNameToCommand["HSETNX"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        sNameToCommand["HMSET"] = shared_ptr<Command>(new HSetCommand(HSetCommand::MSET));
        sNameToCommand["HMSET"]->setCtx(CommandCtx({1}, IMapCommon::HASH, CommandCtx::EVERY_OTHER));
        sNameToCommand["HGET"] = shared_ptr<Command>(new HGetCommand());
        sNameToCommand["HGET"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        sNameToCommand["HEXISTS"] = shared_ptr<Command>(new HExistsCommand());
        sNameToCommand["HEXISTS"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        sNameToCommand["HDEL"] = shared_ptr<Command>(new HDelCommand());
        sNameToCommand["HDEL"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        sNameToCommand["HGETALL"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::GETALL));
        sNameToCommand["HGETALL"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        sNameToCommand["HKEYS"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::KEYS));
        sNameToCommand["HKEYS"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        sNameToCommand["HVALS"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::VALS));
        sNameToCommand["HVALS"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        sNameToCommand["HMGET"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::MGET));
        sNameToCommand["HMGET"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        sNameToCommand["HLEN"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::LEN));
        sNameToCommand["HLEN"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        sNameToCommand["HINCRBY"] = shared_ptr<Command>(new HIncrByCommand(HIncrByCommand::INCRBY));
        sNameToCommand["HINCRBY"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        sNameToCommand["HINCRBYFLOAT"] =
            shared_ptr<Command>(new HIncrByCommand(HIncrByCommand::INCRBYFLOAT));
        sNameToCommand["HINCRBYFLOAT"]->setCtx(CommandCtx({1}, IMapCommon::HASH));
        
        // List Commands
        sNameToCommand["RPUSH"] = shared_ptr<Command>(new LPushCommand(LPushCommand::RPUSH));
        sNameToCommand["RPUSH"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["RPUSHX"] = shared_ptr<Command>(new LPushCommand(LPushCommand::RPUSHX));
        sNameToCommand["RPUSHX"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["LPUSH"] = shared_ptr<Command>(new LPushCommand(LPushCommand::LPUSH));
        sNameToCommand["LPUSH"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["LPUSHX"] = shared_ptr<Command>(new LPushCommand(LPushCommand::LPUSHX));
        sNameToCommand["LPUSHX"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["LLEN"] = shared_ptr<Command>(new LGetCommand(LGetCommand::LEN));
        sNameToCommand["LLEN"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["LINDEX"] = shared_ptr<Command>(new LGetCommand(LGetCommand::INDEX));
        sNameToCommand["LINDEX"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["LPOP"] = shared_ptr<Command>(new LRemCommand(LRemCommand::LPOP));
        sNameToCommand["LPOP"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["RPOP"] = shared_ptr<Command>(new LRemCommand(LRemCommand::RPOP));
        sNameToCommand["RPOP"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["LREM"] = shared_ptr<Command>(new LRemCommand(LRemCommand::REM));
        sNameToCommand["LREM"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["RPOPLPUSH"] = shared_ptr<Command>(new LRemCommand(LRemCommand::RPOPLPUSH));
        sNameToCommand["RPOPLPUSH"]->setCtx(CommandCtx({1, 2}, IMapCommon::LIST));
        sNameToCommand["LRANGE"] = shared_ptr<Command>(new LRangeCommand());
        sNameToCommand["LRANGE"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["LSET"] = shared_ptr<Command>(new LSetCommand());
        sNameToCommand["LSET"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["LTRIM"] = shared_ptr<Command>(new LTrimCommand());
        sNameToCommand["LTRIM"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["LINSERT"] = shared_ptr<Command>(new LInsertCommand());
        sNameToCommand["LINSERT"]->setCtx(CommandCtx({1}, IMapCommon::LIST));
        sNameToCommand["BLPOP"] = shared_ptr<Command>(new BLCommand(BLCommand::BLPOP));
        sNameToCommand["BLPOP"]->setCtx(CommandCtx({1}, IMapCommon::LIST, CommandCtx::TILL_END_MINUS_ONE));
        sNameToCommand["BRPOP"] = shared_ptr<Command>(new BLCommand(BLCommand::BRPOP));
        sNameToCommand["BRPOP"]->setCtx(CommandCtx({1}, IMapCommon::LIST, CommandCtx::TILL_END_MINUS_ONE));
        sNameToCommand["BRPOPLPUSH"] = shared_ptr<Command>(new BRPopLPushCommand());
        sNameToCommand["BRPOPLPUSH"]->setCtx(CommandCtx({1, 2}, IMapCommon::LIST));

        
        // Set Commands
        sNameToCommand["SADD"] = shared_ptr<Command>(new SAddCommand());
        sNameToCommand["SADD"]->setCtx(CommandCtx({1}, IMapCommon::SET));
        sNameToCommand["SMEMBERS"] = shared_ptr<Command>(new SMembersCommand());
        sNameToCommand["SMEMBERS"]->setCtx(CommandCtx({1}, IMapCommon::SET));
        sNameToCommand["SCARD"] = shared_ptr<Command>(new SCardCommand(SCardCommand::CARD));
        sNameToCommand["SCARD"]->setCtx(CommandCtx({1}, IMapCommon::SET));
        sNameToCommand["SISMEMBER"] = shared_ptr<Command>(new SCardCommand(SCardCommand::ISMEMBER));
        sNameToCommand["SISMEMBER"]->setCtx(CommandCtx({1}, IMapCommon::SET));
        sNameToCommand["SPOP"] = shared_ptr<Command>(new SRemCommand(SRemCommand::POP));
        sNameToCommand["SPOP"]->setCtx(CommandCtx({1}, IMapCommon::SET));
        sNameToCommand["SREM"] = shared_ptr<Command>(new SRemCommand(SRemCommand::REM));
        sNameToCommand["SREM"]->setCtx(CommandCtx({1}, IMapCommon::SET));
        sNameToCommand["SDIFF"] = shared_ptr<Command>(new SDiffCommand(SDiffCommand::DIFF));
        sNameToCommand["SDIFF"]->setCtx(CommandCtx({1}, IMapCommon::SET, CommandCtx::TILL_END));
        sNameToCommand["SDIFFSTORE"] = shared_ptr<Command>(new SDiffCommand(SDiffCommand::DIFFSTORE));
        sNameToCommand["SDIFFSTORE"]->setCtx(CommandCtx({1}, IMapCommon::SET, CommandCtx::TILL_END));
        sNameToCommand["SINTER"] = shared_ptr<Command>(new SInterCommand(SInterCommand::INTER));
        sNameToCommand["SINTER"]->setCtx(CommandCtx({1}, IMapCommon::SET, CommandCtx::TILL_END));
        sNameToCommand["SINTERSTORE"] = shared_ptr<Command>(new SInterCommand(SInterCommand::INTERSTORE));
        sNameToCommand["SINTERSTORE"]->setCtx(CommandCtx({1}, IMapCommon::SET, CommandCtx::TILL_END));
        sNameToCommand["SUNION"] = shared_ptr<Command>(new SUnionCommand(SUnionCommand::UNION));
        sNameToCommand["SUNION"]->setCtx(CommandCtx({1}, IMapCommon::SET, CommandCtx::TILL_END));
        sNameToCommand["SUNIONSTORE"] = shared_ptr<Command>(new SUnionCommand(SUnionCommand::UNIONSTORE));
        sNameToCommand["SUNIONSTORE"]->setCtx(CommandCtx({1}, IMapCommon::SET, CommandCtx::TILL_END));
        sNameToCommand["SMOVE"] = shared_ptr<Command>(new SMoveCommand());
        sNameToCommand["SMOVE"]->setCtx(CommandCtx({1, 2}, IMapCommon::SET));
        sNameToCommand["SRANDMEMBER"] = shared_ptr<Command>(new SRandMemberCommand());
        sNameToCommand["SRANDMEMBER"]->setCtx(CommandCtx({1}, IMapCommon::SET));
        
        // Sorted Set Commands
        sNameToCommand["ZADD"] = shared_ptr<Command>(new ZAddCommand());
        sNameToCommand["ZADD"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZRANGE"] = shared_ptr<Command>(new ZRangeCommand(ZRangeCommand::RANGE));
        sNameToCommand["ZRANGE"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZREVRANGE"] = shared_ptr<Command>(new ZRangeCommand(ZRangeCommand::REVRANGE));
        sNameToCommand["ZREVRANGE"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZINCRBY"] = shared_ptr<Command>(new ZIncrByCommand());
        sNameToCommand["ZINCRBY"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZSCORE"] = shared_ptr<Command>(new ZScoreCommand(ZScoreCommand::SCORE));
        sNameToCommand["ZSCORE"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZRANK"] = shared_ptr<Command>(new ZScoreCommand(ZScoreCommand::RANK));
        sNameToCommand["ZRANK"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZREVRANK"] = shared_ptr<Command>(new ZScoreCommand(ZScoreCommand::REVRANK));
        sNameToCommand["ZREVRANK"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZCOUNT"] = shared_ptr<Command>(new ZCountCommand(ZCountCommand::COUNT));
        sNameToCommand["ZCOUNT"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZLEXCOUNT"] = shared_ptr<Command>(new ZCountCommand(ZCountCommand::LEXCOUNT));
        sNameToCommand["ZLEXCOUNT"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZCARD"] = shared_ptr<Command>(new ZCountCommand(ZCountCommand::CARD));
        sNameToCommand["ZCARD"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZREM"] = shared_ptr<Command>(new ZRemCommand());
        sNameToCommand["ZREM"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZUNIONSTORE"] = shared_ptr<Command>(new ZUnionCommand(ZUnionCommand::UNION));
        sNameToCommand["ZUNIONSTORE"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET, CommandCtx::NUM_KEYS, 2));
        sNameToCommand["ZINTERSTORE"] = shared_ptr<Command>(new ZUnionCommand(ZUnionCommand::INTERSECT));
        sNameToCommand["ZINTERSTORE"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET, CommandCtx::NUM_KEYS, 2));
        sNameToCommand["ZRANGEBYSCORE"] = shared_ptr<Command>(new ZRangeByCommand(ZRangeByCommand::RANGEBYSCORE));
        sNameToCommand["ZRANGEBYSCORE"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        
        sNameToCommand["ZREVRANGEBYSCORE"] = shared_ptr<Command>(new ZRangeByCommand(ZRangeByCommand::REVRANGEBYSCORE));
        sNameToCommand["ZREVRANGEBYSCORE"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        
        sNameToCommand["ZREMRANGEBYSCORE"] = shared_ptr<Command>(new ZRemByCommand(ZRemByCommand::REMRANGEBYSCORE));
        sNameToCommand["ZREMRANGEBYSCORE"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZREMRANGEBYRANK"] = shared_ptr<Command>(new ZRemByCommand(ZRemByCommand::REMRANGEBYRANK));
        sNameToCommand["ZREMRANGEBYRANK"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZREMRANGEBYRANK"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZREMRANGEBYLEX"] = shared_ptr<Command>(new ZRemByCommand(ZRemByCommand::REMRANGEBYLEX));
        sNameToCommand["ZREMRANGEBYLEX"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZRANGEBYLEX"] = shared_ptr<Command>(new ZRangeByLexCommand(ZRangeByLexCommand::RANGEBYLEX));
        sNameToCommand["ZRANGEBYLEX"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        sNameToCommand["ZREVRANGEBYLEX"] = shared_ptr<Command>(new ZRangeByLexCommand(ZRangeByLexCommand::REVRANGEBYLEX));
        sNameToCommand["ZREVRANGEBYLEX"]->setCtx(CommandCtx({1}, IMapCommon::SORTEDSET));
        
        // Key Commands
        sNameToCommand["DEL"] = shared_ptr<Command>(new DelCommand());
        sNameToCommand["DEL"]->setCtx(CommandCtx({1}, IMapCommon::NONE));
        sNameToCommand["EXISTS"] = shared_ptr<Command>(new ExistsCommand());
        sNameToCommand["EXISTS"]->setCtx(CommandCtx({1}, IMapCommon::NONE));
        sNameToCommand["TYPE"] = shared_ptr<Command>(new TypeCommand());
        sNameToCommand["TYPE"]->setCtx(CommandCtx({1}, IMapCommon::NONE));
        sNameToCommand["KEYS"] = shared_ptr<Command>(new KeysCommand());
        sNameToCommand["KEYS"]->setCtx(CommandCtx({1}, IMapCommon::NONE, CommandCtx::PATTERN));
        sNameToCommand["TTL"] = shared_ptr<Command>(new TTLCommand(TTLCommand::TTL));
        sNameToCommand["TTL"]->setCtx(CommandCtx({1}, IMapCommon::NONE));
        sNameToCommand["PTTL"] = shared_ptr<Command>(new TTLCommand(TTLCommand::PTTL));
        sNameToCommand["PTTL"]->setCtx(CommandCtx({1}, IMapCommon::NONE));
        sNameToCommand["PERSIST"] = shared_ptr<Command>(new PersistCommand());
        sNameToCommand["PERSIST"]->setCtx(CommandCtx({1}, IMapCommon::NONE));
        sNameToCommand["RANDOMKEY"] = shared_ptr<Command>(new RandomKeyCommand());
        sNameToCommand["RENAME"] = shared_ptr<Command>(new RenameCommand(RenameCommand::RENAME));
        sNameToCommand["RENAME"]->setCtx(CommandCtx({1, 2}, IMapCommon::NONE));
        sNameToCommand["RENAMENX"] = shared_ptr<Command>(new RenameCommand(RenameCommand::RENAMENX));
        sNameToCommand["RENAMENX"]->setCtx(CommandCtx({1, 2}, IMapCommon::NONE));
        sNameToCommand["EXPIRE"] = shared_ptr<Command>(new ExpireCommand(ExpireCommand::EXPIRE));
        sNameToCommand["EXPIRE"]->setCtx(CommandCtx({1}, IMapCommon::NONE));
        sNameToCommand["PEXPIRE"] = shared_ptr<Command>(new ExpireCommand(ExpireCommand::PEXPIRE));
        sNameToCommand["PEXPIRE"]->setCtx(CommandCtx({1}, IMapCommon::NONE));
        sNameToCommand["EXPIREAT"] = shared_ptr<Command>(new ExpireCommand(ExpireCommand::EXPIREAT));
        sNameToCommand["EXPIREAT"]->setCtx(CommandCtx({1}, IMapCommon::NONE));
        sNameToCommand["PEXPIREAT"] = shared_ptr<Command>(new ExpireCommand(ExpireCommand::PEXPIREAT));
        sNameToCommand["PEXPIREAT"]->setCtx(CommandCtx({1}, IMapCommon::NONE));
        
        // HyperLogLog Commands
        sNameToCommand["PFADD"] = shared_ptr<Command>(new PFAddCommand());
        sNameToCommand["PFADD"]->setCtx(CommandCtx({1}, IMapCommon::HLL));
        sNameToCommand["PFCOUNT"] = shared_ptr<Command>(new PFCountCommand());
        sNameToCommand["PFCOUNT"]->setCtx(CommandCtx({1}, IMapCommon::HLL, CommandCtx::TILL_END));
        sNameToCommand["PFMERGE"] = shared_ptr<Command>(new PFMergeCommand());
        sNameToCommand["PFMERGE"]->setCtx(CommandCtx({1}, IMapCommon::HLL, CommandCtx::TILL_END));
        
        // PubSub Commands
        sNameToCommand["SUBSCRIBE"] = shared_ptr<Command>(new SubscribeCommand());
        sNameToCommand["PUBLISH"] = shared_ptr<Command>(new PublishCommand());
        sNameToCommand["UNSUBSCRIBE"] = shared_ptr<Command>(new UnsubscribeCommand());
        sNameToCommand["PSUBSCRIBE"] = shared_ptr<Command>(new PSubscribeCommand());
        sNameToCommand["PUNSUBSCRIBE"] = shared_ptr<Command>(new PUnsubscribeCommand());
        
        // Transactions Commands
        sNameToCommand["MULTI"] = shared_ptr<Command>(new MultiCommand());
        sNameToCommand["WATCH"] = shared_ptr<Command>(new WatchCommand());
        sNameToCommand["UNWATCH"] = shared_ptr<Command>(new UnwatchCommand());
        sNameToCommand["DISCARD"] = shared_ptr<Command>(new DiscardCommand());
        sNameToCommand["EXEC"] = shared_ptr<Command>(new ExecCommand());
        
        // Scripting Commands
        sNameToCommand["EVAL"] = shared_ptr<Command>(new EvalCommand());
        sNameToCommand["EVALSHA"] = shared_ptr<Command>(new EvalSHACommand());
        sNameToCommand["SCRIPT"] = shared_ptr<Command>(new ScriptCommand());
        
//        cout << sNameToCommand.size() << endl;
    }
    
    string upperCaseCmd = cmdName;
    std::transform(upperCaseCmd.begin(), upperCaseCmd.end(), upperCaseCmd.begin(), ::toupper);
    
    if (sNameToCommand.count(upperCaseCmd) == 0) {
//        cout << "command: " + cmdName << endl;
        throw EInvalidCommand(cmdName);
    }
    
    // Create a clone and return in form of a shared pointer
    return shared_ptr<Command>(sNameToCommand[upperCaseCmd]->clone());
}

void Command::getCommand(const string& request, vector<shared_ptr<Command>>& commands)
{
    // Skip parsing if this is an internal command
    if (request.substr(0, CMD_INTERNAL_PREFIX.length()) == CMD_INTERNAL_PREFIX) {
        commands.push_back(getCommandByName(request));
    }
    
    // Parse the request and concert it to array of parsed tokens
    vector<RedisProtocol::RedisArray> parsedTokens;
    RedisProtocol::parse(request, parsedTokens);
    
    if (parsedTokens.size() == 0) {
        throw EInvalidRequest();
    }
    
    for (int i = 0; i < parsedTokens.size(); ++i) {
        // Concert tokens into a command
        RedisProtocol::RedisArray& tokens = parsedTokens[i];
        shared_ptr<Command> pCmd = getCommandByName(tokens[0].first);
        pCmd->setTokens(tokens);
        
        // Push the created command into the final array
        commands.push_back(pCmd);
    }
}

shared_ptr<Command> Command::getCommand(const CommandResult::ResultArray& tokens)
{
    // Convert tokens into a command and return it
    shared_ptr<Command> pCmd = getCommandByName(tokens[0].first);
    pCmd->setTokens(tokens);
    return pCmd;
}

shared_ptr<Command> Command::getFirstCommand(const string &request)
{
    // Convert request to commands and return the first command
    vector<shared_ptr<Command>> cmds;
    getCommand(request, cmds);
    return cmds[0];
}

//-------------------------------------------------------------------------

Command::Command()
{
    mType = Type::EXTERNAL;
    mSpecialType = SpecialType::NORMAL;
    mTimestamp = (int)time(0);
}

Command::~Command()
{
    mType = Type::EXTERNAL;
    mSpecialType = SpecialType::NORMAL;
    mTimestamp = (int)time(0);
}

bool Command::isKeyTypeValid(InMemoryData& db)
{
    // If command doesn't receive key as argument - return true
    if (!mCtx.hasAnyKey()) {
        return true;
    }

    return mCtx.commandMatchesContainer(db, mTokens);
}

void Command::setTokens(const vector<pair<string, int>> &tokens)
{
    mTokens = tokens;
}

string Command::getName() const
{
    if (mTokens.size() == 0) {
        return "";
    }
    
    // First token is always the name of command
    return mTokens[0].first;
}

void Command::setCtx(CommandCtx ctx)
{
    mCtx = ctx;
}

/*
    This helper function writes the command result either to client session (socket)
    or into the passed pCmdResult param.
 */
static void writeResponse(ClientSession& session,
                          CommandResult* pCmdResult,
                          CommandResultPtr resp)
{
    if (pCmdResult) {
        pCmdResult->appendToMultiArray(resp);
    }
    else {
        session.writeResponse(resp->toRedisResponse());
    }
}

void Command::executeEndToEnd(std::shared_ptr<Command> cmd,
                                     int sessionID,
                                     CommandResult* pCmdResult)
{
    ClientSession& session = Arag::instance().getClientSession(sessionID);
    SessionContext& sessionCtx = session.getContext();
    int selectedDBIndex = sessionCtx.getDatabaseIndex();
    InMemoryData& selectedDB = Database::instance().get(selectedDBIndex);
    
    try {
        // Execute the command

        // Check if this is an internal operation and execute it
        if (cmd->getType() == Command::Type::INTERNAL) {
            ResultType rt = cmd->processInternalCommand();
            if (rt == ResultType::STOP) {
                throw runtime_error("Stop Request");
            }
            if (rt == ResultType::SKIP) {
                return;
            }
        }
        
        // Increment processed commands counter
        Arag::instance().incrByTotalCommandsProcessed(1);
        
        // If the current session is in transaction mode - all commands must be enqueued into
        // transaction queue, unless they are specially marked as "bypass-able".
        if (sessionCtx.isInTransaction() &&
            cmd->getSpecialType() != Command::SpecialType::BYPASS_TRANSACTION_STATE) {
            
            sessionCtx.addToTransactionQueue(cmd);

            // Return "QUEUED" as a response for now
            writeResponse(session,
                          pCmdResult,
                          CommandResultPtr(new CommandResult(redis_const::QUEUED,
                                                             RedisProtocol::SIMPLE_STRING)));
            return;
        }
        
        // Process a pending Blocking List command, if any
        sessionCtx.checkPendingBLCommand();

        // Check if the key type is valid
        if (!cmd->isKeyTypeValid(selectedDB)) {
            throw EWrongKeyType();
        }

        // Delete expired keys, if any
        cmd->deleteExpiredKeys(selectedDB);
        
        // Execute the command and return the response
        CommandResultPtr res = cmd->execute(selectedDB, sessionCtx);
        if (!res->isEmpty() && cmd->getType() != Command::Type::INTERNAL) {
            writeResponse(session, pCmdResult, res);
        }
    }
    catch (AragException& e) {
        if (cmd->getType() != Command::Type::INTERNAL) {
            writeResponse(session,
                          pCmdResult,
                          CommandResultPtr(new CommandResult(e.what(),
                                                             RedisProtocol::ERROR)));
        }
    }
    catch (exception& e) {
        throw runtime_error("Stopping");
    }
}

Command::ResultType Command::processInternalCommand()
{
    const std::string cmd = getName();
    
    if (cmd == command_const::CMD_INTERNAL_STOP || cmd == command_const::CMD_EXTERNAL_EXIT) {
        cout << "Stopping the thread" << endl;
        return ResultType::STOP;
    }
    else
        if (cmd == command_const::CMD_INTERNAL_CLEANUP) {
            cout << "Trigerring cleanup" << endl;
            try {
                //mData.cleanup();
            }
            catch (exception& e) {
                cout << "exception occured: " << e.what() << endl;
            }
            return ResultType::SKIP;
        }
    
    return ResultType::CONTINUE;
}

void Command::deleteExpiredKeys(InMemoryData &db)
{
    mCtx.deleteExpiredKeys(db, mTokens);
}

//----------------------------------------------------------------------------

InternalCommand::InternalCommand(std::string name)
{
    mTokens.push_back(std::make_pair(name, 0));
}

CommandResultPtr InternalCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    throw AragException("This function should never be called");
}

