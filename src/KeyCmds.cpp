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
        KeyMap::RedisArray arr;
        kmap.getKeys(pattern, arr);
        return CommandResultPtr(new CommandResult(arr));
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

        int ttl = expItem.expiration;
        if (expItem.expirationType == IMapCommon::TIMESTAMP) {
            ttl = expItem.expiration - (int)time(0);
            if (ttl < 0) {
                ttl = 0;
            }
        }
        
        switch (mCmdType)
        {
            case TTL:
            {
                if (expItem.timeBase == IMapCommon::MSEC) {
                    ttl = ttl / 1000;
                }
                return CommandResultPtr(new CommandResult(to_string(ttl), RedisProtocol::INTEGER));
            }
                
            case PTTL:
            {
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
            expItem.expirationType = IMapCommon::TIMEOUT;
            if (mCmdType == PEXPIRE || mCmdType == PEXPIREAT) {
                expItem.timeBase = IMapCommon::MSEC;
            }
            if (mCmdType == EXPIREAT || mCmdType == PEXPIREAT) {
                expItem.expirationType = IMapCommon::TIMESTAMP;
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

//-------------------------------------------------------------------------

CommandResultPtr ScanCommand::execute(InMemoryData& db, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        int cursor = Utils::convertToInt(mTokens[1].first);
        string pattern = "";
        int count = 10; // Return max 10 items
        
        if (cmdNum > 2) {
            int pos = 2;
            if (mTokens[pos].first == "MATCH") {
                pattern = mTokens[pos + 1].first;
                pos += 2;
            }

            if (cmdNum > pos && mTokens[pos].first == "COUNT") {
                count = Utils::convertToInt(mTokens[pos + 1].first);
            }
        }

        if (cursor == 0) {
            ctx.setScanCommandStartTime(SessionContext::SCAN);
        }
        
        KeyMap& kmap = db.getKeyMap();
        
        KeyMap::RedisArray secondArray;
        int nextCursor = kmap.getKeys(pattern,
                                      secondArray,
                                      cursor,
                                      ctx.getScanCommandStartTime(SessionContext::SCAN),
                                      cursor + count);
        
        CommandResultPtr response(new CommandResult(CommandResult::MULTI_RESPONSE));

        response->appendToMultiArray(CommandResultPtr(new CommandResult(to_string(nextCursor), RedisProtocol::BULK_STRING)));
        response->appendToMultiArray(CommandResultPtr(new CommandResult(secondArray)));

        return response;
    }
    catch (std::exception& e) {
    }
    
    return CommandResult::redisNULLResult();
}

