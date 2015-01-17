#include "TransactionCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

string MultiCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    try {
        
        ctx.setTransactionState(SessionContext::IN_TRANSACTION);
        
        return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}


//-------------------------------------------------------------------------

DiscardCommand::DiscardCommand()
{
    mSpecialType = SpecialType::BYPASS_TRANSACTION_STATE;
}

string DiscardCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    try {

        ctx.finishTransaction();
        
        return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string WatchCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();

    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        for (int i = 1; i < mTokens.size(); ++i) {
            ctx.watchKey(mTokens[i].first);
        }
        
        return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string UnwatchCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    try {

        ctx.clearWatchedKeys();
        
        return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

ExecCommand::ExecCommand()
{
    mSpecialType = SpecialType::BYPASS_TRANSACTION_STATE;
}

string ExecCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    string response = redis_const::NULL_BULK_STRING;
    
    try {
        
        if (!ctx.isInTransaction() || ctx.isTransactionAborted()) {
            throw runtime_error("Transaction has been aborted");
        }
        
        ctx.setTransactionState(SessionContext::TransactionState::NO_TRANSACTION);
        
        const list<shared_ptr<Command>>& cmds = ctx.getTransactionQueue();
        vector<string> responses;
        
        for (auto cmdIter = cmds.begin(); cmdIter != cmds.end(); ++cmdIter) {
            Command::executeEndToEnd(*cmdIter, ctx.getSessionID(), &responses);
        }
        
        response = RedisProtocol::serializeArrayWithPreparedItems(responses);
    }
    catch (std::exception& e) {
    }
    
    ctx.finishTransaction();

    return response;
}

