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
        
        return CommandResult::redisOKResult();
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
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
        
        return CommandResult::redisOKResult();
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr WatchCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();

    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        for (int i = 1; i < mTokens.size(); ++i) {
            ctx.watchKey(mTokens[i].first);
        }
        
        return CommandResult::redisOKResult();
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr UnwatchCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    try {

        ctx.clearWatchedKeys();
        
        return CommandResult::redisOKResult();
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
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
        
        // If we are not in transaction or transaction has been aborted - fail
        if (!ctx.isInTransaction() || ctx.isTransactionAborted()) {
            throw runtime_error("Transaction has been aborted");
        }

        // Set this so that following commands can be executed properly
        ctx.setTransactionState(SessionContext::TransactionState::NO_TRANSACTION);
        
        const list<shared_ptr<Command>>& cmds = ctx.getTransactionQueue();
        
        response = CommandResultPtr(new CommandResult(CommandResult::MULTI_RESPONSE));

        // Run all commands in the transaction queue
        for (auto cmdIter = cmds.begin(); cmdIter != cmds.end(); ++cmdIter) {
            Command::executeEndToEnd(*cmdIter, ctx.getSessionID(), response.get());
        }
    }
    catch (std::exception& e) {
        response = CommandResult::redisNULLResult();
    }

    // Reset all transaction variables
    ctx.finishTransaction();

    return response;
}

