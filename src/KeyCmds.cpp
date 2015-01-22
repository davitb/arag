#include "stdlib.h"
#include <algorithm>
#include "KeyCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "Database.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

CommandResultPtr DelCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        if (mTokens.size() % 2 != 0) {
            throw EInvalidArgument();
        }
        
        int numRemoved = 0;
        
        for (int i = 1; i < mTokens.size(); ++i) {
            int removed = db.delKey(mTokens[i].first);
            if (removed != 0) {
                FIRE_EVENT(EventPublisher::Event::del, mTokens[i].first);
            }
            numRemoved += removed;
        }
        
        return CommandResultPtr(new CommandResult(to_string(numRemoved),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr ExistsCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }

        const string& key = mTokens[1].first;
        
        int ret = db.keyExists(key) ? 1 : 0;
        
        return CommandResultPtr(new CommandResult(to_string(ret),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}
