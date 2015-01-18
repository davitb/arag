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
#include "RedisProtocol.h"
#include "Utils.h"
#include "AragServer.h"
#include <iostream>
#include <regex>

using namespace std;
using namespace arag;
using namespace arag::command_const;

static shared_ptr<Command> getCommandByName(const string& cmdName)
{
    static unordered_map<string, shared_ptr<Command>> sNameToCommand;
    
    if (sNameToCommand.empty()) {
        // Internal Commands
        sNameToCommand[CMD_INTERNAL_STOP] = shared_ptr<Command>(new InternalCommand(CMD_INTERNAL_STOP));
        sNameToCommand[CMD_INTERNAL_CLEANUP] = shared_ptr<Command>(new InternalCommand(CMD_INTERNAL_CLEANUP));

        // Connection Commands
        sNameToCommand["PING"] = shared_ptr<Command>(new PingCommand(PingCommand::CmdType::PING));
        sNameToCommand["ECHO"] = shared_ptr<Command>(new PingCommand(PingCommand::CmdType::ECHO_CMD));
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
        sNameToCommand["SET"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["GET"] = shared_ptr<Command>(new GetCommand());
        sNameToCommand["GET"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["GETSET"] = shared_ptr<Command>(new GetSetCommand());
        sNameToCommand["GETSET"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["APPEND"] = shared_ptr<Command>(new AppendCommand());
        sNameToCommand["APPEND"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["INCR"] = shared_ptr<Command>(new IncrCommand());
        sNameToCommand["INCR"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["GETRANGE"] = shared_ptr<Command>(new GetRangeCommand());
        sNameToCommand["GETRANGE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["SETRANGE"] = shared_ptr<Command>(new SetRangeCommand());
        sNameToCommand["SETRANGE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["MGET"] = shared_ptr<Command>(new MGetCommand());
        sNameToCommand["MGET"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["MSET"] = shared_ptr<Command>(new MSetCommand(false));
        sNameToCommand["MSET"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["MSETNX"] = shared_ptr<Command>(new MSetCommand(true));
        sNameToCommand["MSETNX"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["BITCOUNT"] = shared_ptr<Command>(new BitCountCommand());
        sNameToCommand["BITCOUNT"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["BITOP"] = shared_ptr<Command>(new BitOpCommand());
        sNameToCommand["BITOP"]->setCommandContext(Command::Context(2, InMemoryData::ContainerType::STRING));
        sNameToCommand["BITPOS"] = shared_ptr<Command>(new BitPosCommand());
        sNameToCommand["BITPOS"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["GETBIT"] = shared_ptr<Command>(new GetBitCommand());
        sNameToCommand["GETBIT"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["SETBIT"] = shared_ptr<Command>(new SetBitCommand());
        sNameToCommand["SETBIT"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["STRLEN"] = shared_ptr<Command>(new StrlenCommand());
        sNameToCommand["STRLEN"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["INCRBY"] = shared_ptr<Command>(new IncrByCommand());
        sNameToCommand["INCRBY"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["INCRBYFLOAT"] = shared_ptr<Command>(new IncrByFloatCommand());
        sNameToCommand["INCRBYFLOAT"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["DECR"] = shared_ptr<Command>(new DecrCommand());
        sNameToCommand["DECR"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        sNameToCommand["DECRBY"] = shared_ptr<Command>(new DecrByCommand());
        sNameToCommand["DECRBY"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::STRING));
        
        // Hash Commands
        sNameToCommand["HSET"] = shared_ptr<Command>(new HSetCommand(HSetCommand::CmdType::SET));
        sNameToCommand["HSET"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HSETNX"] = shared_ptr<Command>(new HSetCommand(HSetCommand::CmdType::SETNX));
        sNameToCommand["HSETNX"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HMSET"] = shared_ptr<Command>(new HSetCommand(HSetCommand::CmdType::MSET));
        sNameToCommand["HMSET"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HGET"] = shared_ptr<Command>(new HGetCommand());
        sNameToCommand["HGET"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HEXISTS"] = shared_ptr<Command>(new HExistsCommand());
        sNameToCommand["HEXISTS"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HDEL"] = shared_ptr<Command>(new HDelCommand());
        sNameToCommand["HDEL"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HGETALL"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::CmdType::GETALL));
        sNameToCommand["HGETALL"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HKEYS"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::CmdType::KEYS));
        sNameToCommand["HKEYS"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HVALS"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::CmdType::VALS));
        sNameToCommand["HVALS"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HMGET"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::CmdType::MGET));
        sNameToCommand["HMGET"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HLEN"] = shared_ptr<Command>(new HGetAllCommand(HGetAllCommand::CmdType::LEN));
        sNameToCommand["HLEN"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HINCRBY"] = shared_ptr<Command>(new HIncrByCommand(HIncrByCommand::CmdType::INCRBY));
        sNameToCommand["HINCRBY"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        sNameToCommand["HINCRBYFLOAT"] =
            shared_ptr<Command>(new HIncrByCommand(HIncrByCommand::CmdType::INCRBYFLOAT));
        sNameToCommand["HINCRBYFLOAT"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::HASH));
        
        // List Commands
        sNameToCommand["RPUSH"] = shared_ptr<Command>(new LPushCommand(LPushCommand::CmdType::RPUSH));
        sNameToCommand["RPUSH"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["RPUSHX"] = shared_ptr<Command>(new LPushCommand(LPushCommand::CmdType::RPUSHX));
        sNameToCommand["RPUSHX"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["LPUSH"] = shared_ptr<Command>(new LPushCommand(LPushCommand::CmdType::LPUSH));
        sNameToCommand["LPUSH"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["LPUSHX"] = shared_ptr<Command>(new LPushCommand(LPushCommand::CmdType::LPUSHX));
        sNameToCommand["LPUSHX"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["LLEN"] = shared_ptr<Command>(new LGetCommand(LGetCommand::CmdType::LEN));
        sNameToCommand["LLEN"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["LINDEX"] = shared_ptr<Command>(new LGetCommand(LGetCommand::CmdType::INDEX));
        sNameToCommand["LINDEX"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["LPOP"] = shared_ptr<Command>(new LRemCommand(LRemCommand::CmdType::LPOP));
        sNameToCommand["LPOP"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["RPOP"] = shared_ptr<Command>(new LRemCommand(LRemCommand::CmdType::RPOP));
        sNameToCommand["RPOP"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["LREM"] = shared_ptr<Command>(new LRemCommand(LRemCommand::CmdType::REM));
        sNameToCommand["LREM"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["RPOPLPUSH"] = shared_ptr<Command>(new LRemCommand(LRemCommand::CmdType::RPOPLPUSH));
        sNameToCommand["RPOPLPUSH"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["LRANGE"] = shared_ptr<Command>(new LRangeCommand());
        sNameToCommand["LRANGE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["LSET"] = shared_ptr<Command>(new LSetCommand());
        sNameToCommand["LSET"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["LTRIM"] = shared_ptr<Command>(new LTrimCommand());
        sNameToCommand["LTRIM"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["LINSERT"] = shared_ptr<Command>(new LInsertCommand());
        sNameToCommand["LINSERT"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["BLPOP"] = shared_ptr<Command>(new BLCommand(BLCommand::BLPOP));
        sNameToCommand["BLPOP"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["BRPOP"] = shared_ptr<Command>(new BLCommand(BLCommand::BRPOP));
        sNameToCommand["BRPOP"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["BRPOPLPUSH"] = shared_ptr<Command>(new BRPopLPushCommand());
        sNameToCommand["BRPOPLPUSH"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));

        
        // Set Commands
        sNameToCommand["SADD"] = shared_ptr<Command>(new SAddCommand());
        sNameToCommand["SADD"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SET));
        sNameToCommand["SMEMBERS"] = shared_ptr<Command>(new SMembersCommand());
        sNameToCommand["SMEMBERS"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SCARD"] = shared_ptr<Command>(new SCardCommand(SCardCommand::CmdType::CARD));
        sNameToCommand["SCARD"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SISMEMBER"] = shared_ptr<Command>(new SCardCommand(SCardCommand::CmdType::ISMEMBER));
        sNameToCommand["SISMEMBER"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SPOP"] = shared_ptr<Command>(new SRemCommand(SRemCommand::CmdType::POP));
        sNameToCommand["SPOP"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SREM"] = shared_ptr<Command>(new SRemCommand(SRemCommand::CmdType::REM));
        sNameToCommand["SREM"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SDIFF"] = shared_ptr<Command>(new SDiffCommand(SDiffCommand::CmdType::DIFF));
        sNameToCommand["SDIFF"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SDIFFSTORE"] = shared_ptr<Command>(new SDiffCommand(SDiffCommand::CmdType::DIFFSTORE));
        sNameToCommand["SDIFFSTORE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SINTER"] = shared_ptr<Command>(new SInterCommand(SInterCommand::CmdType::INTER));
        sNameToCommand["SINTER"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SINTERSTORE"] = shared_ptr<Command>(new SInterCommand(SInterCommand::CmdType::INTERSTORE));
        sNameToCommand["SINTERSTORE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SUNION"] = shared_ptr<Command>(new SUnionCommand(SUnionCommand::CmdType::UNION));
        sNameToCommand["SUNION"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SUNIONSTORE"] = shared_ptr<Command>(new SUnionCommand(SUnionCommand::CmdType::UNIONSTORE));
        sNameToCommand["SUNIONSTORE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SMOVE"] = shared_ptr<Command>(new SMoveCommand());
        sNameToCommand["SMOVE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        sNameToCommand["SRANDMEMBER"] = shared_ptr<Command>(new SRandMemberCommand());
        sNameToCommand["SRANDMEMBER"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::LIST));
        
        // Sorted Set Commands
        sNameToCommand["ZADD"] = shared_ptr<Command>(new ZAddCommand());
        sNameToCommand["ZADD"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZRANGE"] = shared_ptr<Command>(new ZRangeCommand(ZRangeCommand::CmdType::RANGE));
        sNameToCommand["ZRANGE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZREVRANGE"] = shared_ptr<Command>(new ZRangeCommand(ZRangeCommand::CmdType::REVRANGE));
        sNameToCommand["ZREVRANGE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZINCRBY"] = shared_ptr<Command>(new ZIncrByCommand());
        sNameToCommand["ZINCRBY"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZSCORE"] = shared_ptr<Command>(new ZScoreCommand(ZScoreCommand::CmdType::SCORE));
        sNameToCommand["ZSCORE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZRANK"] = shared_ptr<Command>(new ZScoreCommand(ZScoreCommand::CmdType::RANK));
        sNameToCommand["ZRANK"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZREVRANK"] = shared_ptr<Command>(new ZScoreCommand(ZScoreCommand::CmdType::REVRANK));
        sNameToCommand["ZREVRANK"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZCOUNT"] = shared_ptr<Command>(new ZCountCommand(ZCountCommand::CmdType::COUNT));
        sNameToCommand["ZCOUNT"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZLEXCOUNT"] = shared_ptr<Command>(new ZCountCommand(ZCountCommand::CmdType::LEXCOUNT));
        sNameToCommand["ZLEXCOUNT"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZCARD"] = shared_ptr<Command>(new ZCountCommand(ZCountCommand::CmdType::CARD));
        sNameToCommand["ZCARD"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZREM"] = shared_ptr<Command>(new ZRemCommand(ZRemCommand::CmdType::REM));
        sNameToCommand["ZREM"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZUNIONSTORE"] = shared_ptr<Command>(new ZUnionCommand(ZUnionCommand::CmdType::UNION));
        sNameToCommand["ZINTERSTORE"] = shared_ptr<Command>(new ZUnionCommand(ZUnionCommand::CmdType::INTERSECT));
        sNameToCommand["ZRANGEBYSCORE"] = shared_ptr<Command>(new ZRangeByCommand(ZRangeByCommand::CmdType::RANGEBYSCORE));
        sNameToCommand["ZRANGEBYSCORE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZREVRANGEBYSCORE"] = shared_ptr<Command>(new ZRangeByCommand(ZRangeByCommand::CmdType::REVRANGEBYSCORE));
        sNameToCommand["ZREVRANGEBYSCORE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZREMRANGEBYSCORE"] = shared_ptr<Command>(new ZRemByCommand(ZRemByCommand::CmdType::REMRANGEBYSCORE));
        sNameToCommand["ZREMRANGEBYSCORE"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZREMRANGEBYRANK"] = shared_ptr<Command>(new ZRemByCommand(ZRemByCommand::CmdType::REMRANGEBYRANK));
        sNameToCommand["ZREMRANGEBYRANK"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZREMRANGEBYLEX"] = shared_ptr<Command>(new ZRemByCommand(ZRemByCommand::CmdType::REMRANGEBYLEX));
        sNameToCommand["ZREMRANGEBYLEX"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZRANGEBYLEX"] = shared_ptr<Command>(new ZRangeByLexCommand(ZRangeByLexCommand::CmdType::RANGEBYLEX));
        sNameToCommand["ZRANGEBYLEX"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        sNameToCommand["ZREVRANGEBYLEX"] = shared_ptr<Command>(new ZRangeByLexCommand(ZRangeByLexCommand::CmdType::REVRANGEBYLEX));
        sNameToCommand["ZREVRANGEBYLEX"]->setCommandContext(Command::Context(1, InMemoryData::ContainerType::SORTEDSET));
        
        // Key Commands
        sNameToCommand["DEL"] = shared_ptr<Command>(new DelCommand());
        sNameToCommand["EXISTS"] = shared_ptr<Command>(new ExistsCommand());
        
        // HyperLogLog Commands
        sNameToCommand["PFADD"] = shared_ptr<Command>(new PFAddCommand());
        sNameToCommand["PFCOUNT"] = shared_ptr<Command>(new PFCountCommand());
        sNameToCommand["PFMERGE"] = shared_ptr<Command>(new PFMergeCommand());
        
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
    }
    
    string upperCaseCmd = cmdName;
    std::transform(upperCaseCmd.begin(), upperCaseCmd.end(), upperCaseCmd.begin(), ::toupper);
    
    if (sNameToCommand.count(upperCaseCmd) == 0) {
        cout << "command: " + cmdName << endl;
        throw invalid_argument("Invalid command: " + cmdName);
    }
    
    return shared_ptr<Command>(sNameToCommand[upperCaseCmd]->clone());
}

void Command::getCommand(const string& cmdline, vector<shared_ptr<Command>>& commands)
{
    // Skip parsing if this is an internal command
    if (cmdline.substr(0, CMD_INTERNAL_PREFIX.length()) == CMD_INTERNAL_PREFIX) {
        commands.push_back(getCommandByName(cmdline));
    }
    
    vector<RedisProtocol::RedisArray> parsedCommands;
    
    RedisProtocol::parse(cmdline, parsedCommands);
    
    if (parsedCommands.size() == 0) {
        throw invalid_argument("Invalid Command");
    }
    
    for (int i = 0; i < parsedCommands.size(); ++i) {
        RedisProtocol::RedisArray& tokens = parsedCommands[i];
        shared_ptr<Command> pCmd = getCommandByName(tokens[0].first);
        
        pCmd->setTokens(tokens);
        
        commands.push_back(pCmd);
    }
}

shared_ptr<Command> Command::getCommand(const string &cmdline)
{
    vector<shared_ptr<Command>> cmds;
    getCommand(cmdline, cmds);
    return cmds[0];
}

//-------------------------------------------------------------------------

Command::Command()
{
    mType = Type::EXTERNAL;
    mSpecialType = SpecialType::NORMAL;
    mTimestamp = (int)time(0);
}

bool Command::isKeyTypeValid(InMemoryData& db)
{
    if (mCtx.mKeyArgIndex == -1) {
        return true;
    }

    InMemoryData::ContainerType type = db.getKeyType(mTokens[mCtx.mKeyArgIndex].first);
    if (type == InMemoryData::ContainerType::NONE) {
        return true;
    }
    
    return (type == mCtx.mContainerType);
}

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

void Command::setCommandContext(Command::Context ctx)
{
    mCtx = ctx;
}

static void writeResponse(ClientSession& session,
                          std::vector<std::string>* pResponeList,
                          const string& resp)
{
    if (pResponeList) {
        pResponeList->push_back(resp);
    }
    else {
        session.writeResponse(resp);
    }
}

void Command::executeEndToEnd(std::shared_ptr<Command> cmd,
                                     int sessionID,
                                     std::vector<std::string>* pResponeList)
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
        
        if (sessionCtx.isInTransaction() &&
            cmd->getSpecialType() != Command::SpecialType::BYPASS_TRANSACTION_STATE) {
            
            sessionCtx.addToTransactionQueue(cmd);

            writeResponse(session,
                          pResponeList,
                          RedisProtocol::serializeNonArray("QUEUED", RedisProtocol::DataType::SIMPLE_STRING));
            return;
        }
        
        // Process a pending Blocking List command if any
        sessionCtx.checkPendingBLCommand();

        if (!cmd->isKeyTypeValid(selectedDB)) {
            throw invalid_argument("Wrong key operation");
        }
        
        string res = cmd->execute(selectedDB, sessionCtx);
        
        if (res.length() != 0 && cmd->getType() != Command::Type::INTERNAL) {
            writeResponse(session, pResponeList, res);
        }
    }
    catch (invalid_argument& e) {
        if (cmd->getType() != Command::Type::INTERNAL) {
            writeResponse(session, pResponeList, redis_const::ERR_GENERIC);
        }
    }
    catch (exception& e) {
        throw runtime_error("Stopping");
    }
}

Command::ResultType Command::processInternalCommand()
{
    const std::string cmd = getCommandName();
    
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


//----------------------------------------------------------------------------

InternalCommand::InternalCommand(std::string name)
{
    mTokens.push_back(std::make_pair(name, 0));
}

