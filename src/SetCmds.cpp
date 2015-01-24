#include "stdlib.h"
#include <algorithm>
#include "SetCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "Database.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

CommandResultPtr SAddCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        
        SetMap& setMap = data.getSetMap();
        
        int numAdded = 0;
        
        for (int i = 2; i < mTokens.size(); ++i) {
            if (setMap.add(key, mTokens[i].first)) {
                FIRE_EVENT(EventPublisher::Event::set_new, key);
                numAdded++;
            }
        }
        
        FIRE_EVENT(EventPublisher::Event::sadd, key);
        
        return CommandResultPtr(new CommandResult(to_string(numAdded),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SMembersCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        
        SetMap& setMap = data.getSetMap();
        
        return CommandResultPtr(new CommandResult(setMap.getMembers(key)));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SCardCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        
        SetMap& setMap = data.getSetMap();

        int ret = 0;
        
        switch (mCmdType)
        {
            case CARD:
            {
                ret = setMap.size(key);
                break;
            }
                
            case ISMEMBER:
            {
                if (cmdNum != Consts::MAX_ARG_NUM) {
                    throw EInvalidArgument();
                }
                
                const string& member = mTokens[2].first;
                ret = setMap.isMember(key, member);
                break;
            }
        }
        
        return CommandResultPtr(new CommandResult(to_string(ret),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SRemCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        
        SetMap& setMap = data.getSetMap();
        
        int ret = 0;
        
        switch (mCmdType)
        {
            case REM:
            {
                if (cmdNum < Consts::MIN_ARG_NUM + 1) {
                    throw EInvalidArgument();
                }
                
                int numRemoved = 0;
                
                for (int i = 2; i < mTokens.size(); ++i) {
                    numRemoved += setMap.rem(key, mTokens[i].first);
                }
                
                if (!setMap.keyExists(key)) {
                    FIRE_EVENT(EventPublisher::Event::del, key);
                }
                FIRE_EVENT(EventPublisher::Event::srem, key);
                
                return CommandResultPtr(new CommandResult(to_string(numRemoved),
                                                        RedisProtocol::INTEGER));
            }
                
            case POP:
            {
                string randMember = setMap.getRandMember(key);
                setMap.rem(key, randMember);
                
                if (!setMap.keyExists(key)) {
                    FIRE_EVENT(EventPublisher::Event::del, key);
                }
                FIRE_EVENT(EventPublisher::Event::spop, key);
                
                return CommandResultPtr(new CommandResult(randMember,
                                                          RedisProtocol::BULK_STRING));
            }
        }
        
        return CommandResultPtr(new CommandResult(to_string(ret),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SDiffCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        SetMap& setMap = data.getSetMap();
        
        int ret = 0;
        
        switch (mCmdType)
        {
            case DIFF:
            {
                const string& key = mTokens[1].first;
                
                vector<string> diffKeys;
                for (int i = 2; i < mTokens.size(); ++i) {
                    diffKeys.push_back(mTokens[i].first);
                }
                
                SetMap::SetType newSet;
                setMap.diff(key, diffKeys, newSet);
                
                vector<std::pair<std::string, int>> retArray;
                
                for (auto val : newSet) {
                    retArray.push_back(make_pair(val, RedisProtocol::BULK_STRING));
                }
                
                return CommandResultPtr(new CommandResult(retArray));
            }
                
            case DIFFSTORE:
            {
                if (cmdNum < Consts::MIN_ARG_NUM + 1) {
                    throw EInvalidArgument();
                }
                
                const string& destKey = mTokens[1].first;
                const string& sourceKey = mTokens[2].first;
                
                vector<string> diffKeys;
                for (int i = 3; i < mTokens.size(); ++i) {
                    diffKeys.push_back(mTokens[i].first);
                }
                
                if (setMap.keyExists(destKey)) {
                    FIRE_EVENT(EventPublisher::Event::del, destKey);
                }
                
                int num = setMap.diff(destKey, sourceKey, diffKeys);

                FIRE_EVENT(EventPublisher::Event::sunionostore, destKey);
                
                return CommandResultPtr(new CommandResult(to_string(num),
                                                          RedisProtocol::INTEGER));
            }
        }
        
        return CommandResultPtr(new CommandResult(to_string(ret),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SInterCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        SetMap& setMap = data.getSetMap();
        
        int ret = 0;
        
        switch (mCmdType)
        {
            case INTER:
            {
                vector<string> keys;
                for (int i = 1; i < mTokens.size(); ++i) {
                    keys.push_back(mTokens[i].first);
                }
                
                SetMap::SetType newSet;
                setMap.inter(keys, newSet);
                
                vector<std::pair<std::string, int>> retArray;
                
                for (auto val : newSet) {
                    retArray.push_back(make_pair(val, RedisProtocol::BULK_STRING));
                }
                
                return CommandResultPtr(new CommandResult(retArray));
            }
                
            case INTERSTORE:
            {
                if (cmdNum < Consts::MIN_ARG_NUM + 1) {
                    throw EInvalidArgument();
                }
                
                const string& destKey = mTokens[1].first;
                
                vector<string> keys;
                for (int i = 2; i < mTokens.size(); ++i) {
                    keys.push_back(mTokens[i].first);
                }
                
                if (!setMap.keyExists(destKey)) {
                    FIRE_EVENT(EventPublisher::Event::del, destKey);
                }
                
                int num = setMap.inter(destKey, keys);
                
                FIRE_EVENT(EventPublisher::Event::sinterstore, destKey);
                
                return CommandResultPtr(new CommandResult(to_string(num),
                                                          RedisProtocol::INTEGER));
            }
        }
        
        return CommandResultPtr(new CommandResult(to_string(ret),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SUnionCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        SetMap& setMap = data.getSetMap();
        
        int ret = 0;
        
        switch (mCmdType)
        {
            case UNION:
            {
                vector<string> keys;
                for (int i = 1; i < mTokens.size(); ++i) {
                    keys.push_back(mTokens[i].first);
                }
                
                SetMap::SetType newSet;
                setMap.uni(keys, newSet);
                
                vector<std::pair<std::string, int>> retArray;
                
                for (auto val : newSet) {
                    retArray.push_back(make_pair(val, RedisProtocol::BULK_STRING));
                }
                
                return CommandResultPtr(new CommandResult(retArray));
            }
                
            case UNIONSTORE:
            {
                if (cmdNum < Consts::MIN_ARG_NUM + 1) {
                    throw EInvalidArgument();
                }
                
                const string& destKey = mTokens[1].first;
                
                vector<string> keys;
                for (int i = 2; i < mTokens.size(); ++i) {
                    keys.push_back(mTokens[i].first);
                }
                
                if (!setMap.keyExists(destKey)) {
                    FIRE_EVENT(EventPublisher::Event::del, destKey);
                }
                
                int num = setMap.uni(destKey, keys);
                
                FIRE_EVENT(EventPublisher::Event::sunionostore, destKey);
                
                return CommandResultPtr(new CommandResult(to_string(num),
                                                          RedisProtocol::INTEGER));
            }
        }
        
        return CommandResultPtr(new CommandResult(to_string(ret),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SMoveCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        SetMap& setMap = data.getSetMap();
        
        const string& source = mTokens[1].first;
        const string& dest = mTokens[2].first;
        const string& member = mTokens[3].first;
        
        int ret = setMap.move(source, dest, member);
        
        FIRE_EVENT(EventPublisher::Event::srem, source);
        FIRE_EVENT(EventPublisher::Event::sadd, dest);
        FIRE_EVENT(EventPublisher::Event::set_new, dest);
        
        return CommandResultPtr(new CommandResult(to_string(ret),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SRandMemberCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        SetMap& setMap = data.getSetMap();
        
        const string& key = mTokens[1].first;
        
        if (cmdNum == Consts::MIN_ARG_NUM) {
            string member = setMap.getRandMember(key);
            return CommandResultPtr(new CommandResult(member,
                                                      RedisProtocol::BULK_STRING));
        }
        
        int count = Utils::convertToInt(mTokens[2].first);
        
        vector<pair<string, int>> res;
        if (count < 0) {
            for (int i = 1; i <= abs(count); ++i) {
                res.push_back(make_pair(setMap.getRandMember(key),
                                        RedisProtocol::BULK_STRING));
            }
        }
        else {
            res = setMap.getRandMembers(key, count);
        }
        
        return CommandResultPtr(new CommandResult(res));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}
