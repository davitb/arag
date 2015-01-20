#include "TransactionCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

CommandResultPtr MultiCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    try {
        
        ctx.setTransactionState(SessionContext::IN_TRANSACTION);
        
        return CommandResultPtr(new CommandResult("OK", RedisProtocol::DataType::SIMPLE_STRING));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}


//-------------------------------------------------------------------------

DiscardCommand::DiscardCommand()
{
    mSpecialType = SpecialType::BYPASS_TRANSACTION_STATE;
}

CommandResultPtr DiscardCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    try {

        ctx.finishTransaction();
        
        return CommandResultPtr(new CommandResult("OK", RedisProtocol::DataType::SIMPLE_STRING));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr WatchCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();

    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        for (int i = 1; i < mTokens.size(); ++i) {
            ctx.watchKey(mTokens[i].first);
        }
        
        return CommandResultPtr(new CommandResult("OK", RedisProtocol::DataType::SIMPLE_STRING));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr UnwatchCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    try {

        ctx.clearWatchedKeys();
        
        return CommandResultPtr(new CommandResult("OK", RedisProtocol::DataType::SIMPLE_STRING));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

ExecCommand::ExecCommand()
{
    mSpecialType = SpecialType::BYPASS_TRANSACTION_STATE;
}

CommandResultPtr ExecCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    CommandResultPtr response;
    
    try {
        
        if (!ctx.isInTransaction() || ctx.isTransactionAborted()) {
            throw runtime_error("Transaction has been aborted");
        }
        
        ctx.setTransactionState(SessionContext::TransactionState::NO_TRANSACTION);
        
        const list<shared_ptr<Command>>& cmds = ctx.getTransactionQueue();
        
        response = CommandResultPtr(new CommandResult(CommandResult::MULTI_RESPONSE));
        
        for (auto cmdIter = cmds.begin(); cmdIter != cmds.end(); ++cmdIter) {
            Command::executeEndToEnd(*cmdIter, ctx.getSessionID(), response.get());
        }
    }
    catch (std::exception& e) {
        response = CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::NILL));
    }
    
    ctx.finishTransaction();

    return response;
}

