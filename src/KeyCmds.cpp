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
        
        KeyMap& kmap = db.getKeyMap();
        
        for (int i = 1; i < mTokens.size(); ++i) {
            int removed = kmap.delKey(mTokens[i].first);
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
        
        KeyMap& kmap = db.getKeyMap();
        
        int ret = kmap.keyExists(key) ? 1 : 0;
        
        return CommandResultPtr(new CommandResult(to_string(ret),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr TypeCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        
        KeyMap& kmap = db.getKeyMap();
        
        return CommandResultPtr(new CommandResult(kmap.getContainerName(key),
                                                  RedisProtocol::SIMPLE_STRING));
    }
    catch (std::exception& e) {
    }
    
    return CommandResult::redisNULLResult();
}

//-------------------------------------------------------------------------

CommandResultPtr KeysCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& pattern = mTokens[1].first;
        
        KeyMap& kmap = db.getKeyMap();
        
        return CommandResultPtr(new CommandResult(kmap.getKeys(pattern)));
    }
    catch (std::exception& e) {
    }
    
    return CommandResult::redisNULLResult();
}

//-------------------------------------------------------------------------

CommandResultPtr TTLCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        KeyMap::Item expItem;
        
        KeyMap& kmap = db.getKeyMap();

        try {
            expItem = kmap.get(key);
            
            if (expItem.expiration == 0) {
                return CommandResultPtr(new CommandResult("-1", RedisProtocol::INTEGER));
            }
        }
        catch (EInvalidKey) {
            return CommandResultPtr(new CommandResult("-2", RedisProtocol::INTEGER));
        }
        
        switch (mCmdType)
        {
            case TTL:
            {
                int ttl = expItem.expiration;
                if (expItem.timeBase == IMapCommon::MSEC) {
                    ttl = ttl / 1000;
                }
                return CommandResultPtr(new CommandResult(to_string(ttl), RedisProtocol::INTEGER));
            }
                
            case PTTL:
            {
                int ttl = expItem.expiration;
                if (expItem.timeBase == IMapCommon::SEC) {
                    ttl = ttl * 1000;
                }
                return CommandResultPtr(new CommandResult(to_string(ttl), RedisProtocol::INTEGER));
            }
        }
    }
    catch (std::exception& e) {
    }
    
    return CommandResult::redisNULLResult();
}

//-------------------------------------------------------------------------

CommandResultPtr PersistCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        
        KeyMap& kmap = db.getKeyMap();
        
        try {
            KeyMap::Item& expItem = kmap.get(key);
            
            if (expItem.expiration == 0) {
                return CommandResultPtr(new CommandResult("0", RedisProtocol::INTEGER));
            }
            
            expItem.expiration = 0;
            
            return CommandResultPtr(new CommandResult("1", RedisProtocol::INTEGER));
        }
        catch (EInvalidKey) {
            return CommandResultPtr(new CommandResult("0", RedisProtocol::INTEGER));
        }
    }
    catch (std::exception& e) {
    }
    
    return CommandResult::redisNULLResult();
}

//-------------------------------------------------------------------------

CommandResultPtr RandomKeyCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        KeyMap& kmap = db.getKeyMap();
        
        if (kmap.size() != 0) {
            return CommandResultPtr(new CommandResult(kmap.getRandomKey(),
                                                      RedisProtocol::BULK_STRING));
        }
    }
    catch (std::exception& e) {
    }
    
    return CommandResult::redisNULLResult();
}

//-------------------------------------------------------------------------

CommandResultPtr RenameCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        const string& newKey = mTokens[2].first;
        
        KeyMap::Item expItem;
        
        KeyMap& kmap = db.getKeyMap();
        
        if (key == newKey) {
            return CommandResult::redisErrorResult("Keys are the same");
        }
        
        if (!kmap.keyExists(key)) {
            return CommandResult::redisErrorResult("Key doesn't exist");
        }
        
        switch (mCmdType)
        {
            case RENAME:
            {
                kmap.rename(key, newKey);
                
                return CommandResult::redisOKResult();
            }
                
            case RENAMENX:
            {
                if (kmap.keyExists(newKey)) {
                    return CommandResultPtr(new CommandResult("0", RedisProtocol::INTEGER));
                }

                int ret = kmap.rename(key, newKey);
                
                return CommandResultPtr(new CommandResult(to_string(ret), RedisProtocol::INTEGER));
            }
        }
    }
    catch (std::exception& e) {
    }
    
    return CommandResult::redisNULLResult();
}

//-------------------------------------------------------------------------

CommandResultPtr ExpireCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        int expiration = Utils::convertToInt(mTokens[2].first);
        
        KeyMap& kmap = db.getKeyMap();

        try {
            KeyMap::Item& expItem = kmap.get(key);
            
            expItem.expiration = expiration;
            expItem.timeBase = IMapCommon::SEC;
            if (mCmdType == PEXPIRE) {
                expItem.timeBase = IMapCommon::MSEC;
            }
            
            return CommandResultPtr(new CommandResult("1", RedisProtocol::INTEGER));
        }
        catch (EInvalidKey) {
            return CommandResultPtr(new CommandResult("0", RedisProtocol::INTEGER));
        }
        
    }
    catch (std::exception& e) {
    }
    
    return CommandResult::redisNULLResult();
}
