#include "stdlib.h"
#include <algorithm>
#include "SortedSetCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "Database.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

CommandResultPtr ZAddCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        if (mTokens.size() % 2 != 0) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        int numAdded = 0;
        
        for (int i = 2; i < mTokens.size(); i += 2) {
            numAdded += setMap.insert(key, mTokens[i + 1].first,
                                      Utils::convertToDouble(mTokens[i].first));
        }
        
        FIRE_EVENT(EventPublisher::Event::zadd, key);
        
        return CommandResultPtr(new CommandResult(to_string(numAdded),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr ZRangeCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        int start = Utils::convertToInt(mTokens[2].first);
        int end = Utils::convertToInt(mTokens[3].first);
        bool bWithScores = false;
        
        if (cmdNum == Consts::MAX_ARG_NUM) {
            if (mTokens[4].first != "WITHSCORES") {
                throw EInvalidArgument();
            }
            bWithScores = true;
        }
        
        SortedSetMap& setMap = data.getSortedSetMap();

        Utils::normalizeIndexes(start, end, setMap.size(key));
        
        SortedSetMap::RedisArray ret;
        
        switch (mCmdType)
        {
            case RANGE:
            {
                ret = setMap.range(key, start, end, bWithScores);
                break;
            }
                
            case REVRANGE:
            {
                ret = setMap.revRange(key, start, end, bWithScores);
                break;
            }
        }
        
        return CommandResultPtr(new CommandResult(ret));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr ZScoreCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        const string& member = mTokens[2].first;
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        switch (mCmdType)
        {
            case SCORE:
            {
                string score = Utils::dbl2str(setMap.score(key, member));
                return CommandResultPtr(new CommandResult(score,
                                                          RedisProtocol::BULK_STRING));
            }
                
            case RANK:
            {
                int rank = setMap.rank(key, member, false);
                return CommandResultPtr(new CommandResult(to_string(rank),
                                                          RedisProtocol::INTEGER));
            }
                
            case REVRANK:
            {
                int rank = setMap.rank(key, member, true);
                return CommandResultPtr(new CommandResult(to_string(rank),
                                                          RedisProtocol::INTEGER));
            }
        }
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr ZCountCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;

        SortedSetMap& setMap = data.getSortedSetMap();
        
        switch (mCmdType)
        {
            case COUNT:
            {
                if (cmdNum != Consts::MAX_ARG_NUM) {
                    throw EInvalidArgument();
                }
                
                double min = Utils::convertToDoubleByLimit(mTokens[2].first, false);
                double max = Utils::convertToDoubleByLimit(mTokens[3].first, true);
                
                int count = setMap.count(key, min, max);
                
                return CommandResultPtr(new CommandResult(to_string(count),
                                                          RedisProtocol::INTEGER));
            }
                
            case LEXCOUNT:
            {
                if (cmdNum != Consts::MAX_ARG_NUM) {
                    throw EInvalidArgument();
                }
                
                string min = Utils::convertToStringByLimit(mTokens[2].first, false);
                string max = Utils::convertToStringByLimit(mTokens[3].first, true);
                
                int count = setMap.lexCount(key, min, max);
                
                return CommandResultPtr(new CommandResult(to_string(count),
                                                          RedisProtocol::INTEGER));
            }
                
            case CARD:
            {
                int size = setMap.size(key);
                return CommandResultPtr(new CommandResult(to_string(size),
                                                          RedisProtocol::INTEGER));
            }
        }
        
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr ZIncrByCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        double by = Utils::convertToDouble(mTokens[2].first);
        const string& member = mTokens[3].first;
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        string ret = Utils::dbl2str(setMap.incrBy(key, member, by));
        
        FIRE_EVENT(EventPublisher::Event::zincr, key);
        
        return CommandResultPtr(new CommandResult(ret, RedisProtocol::BULK_STRING));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr ZRemCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        if (cmdNum % 2 != 1) {
            throw EInvalidArgument();
        }

        int numRemoved = 0;
        
        for (int i = 2; i < mTokens.size(); i += 2) {
            numRemoved += setMap.rem(key, mTokens[i].first);
        }
        
        if (!setMap.keyExists(key)) {
            FIRE_EVENT(EventPublisher::Event::del, key);
        }
        FIRE_EVENT(EventPublisher::Event::zrem, key);
        
        return CommandResultPtr(new CommandResult(to_string(numRemoved),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::UNKNOWN_ERROR,
                                                  RedisProtocol::ERROR));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr ZUnionCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& dest = mTokens[1].first;
        int numKeys = Utils::convertToInt(mTokens[2].first);

        if (numKeys == 0) {
            throw EInvalidArgument();
        }
        
        int pos = 3;
        if (cmdNum < numKeys + pos) {
            throw EInvalidArgument();
        }
        
        vector<string> keys(numKeys);
        for (int i = 0; i < numKeys; ++i) {
            keys[i] = mTokens[pos + i].first;
        }

        pos += numKeys; // Skip all keys
        
        string aggregate = "SUM";
        bool bWeigthsProvided = false;
        vector<int> weights(numKeys);
        
        // Initialize with default 1s
        for (int i = 0; i < numKeys; ++i) {
            weights[i] = 1;
        }
        
        if (cmdNum > pos) {
            if (mTokens[pos].first != "WEIGHTS" && mTokens[pos].first != "AGGREGATE") {
                throw EInvalidArgument();
            }
            
            if (mTokens[pos].first == "WEIGHTS") {
                if (cmdNum < numKeys + pos) {
                    throw EInvalidArgument();
                }
                
                bWeigthsProvided = true;
                pos += 1; // Skip "WEIGTHS"
                for (int i = 0; i < numKeys; ++i) {
                    weights[i] = Utils::convertToInt(mTokens[pos + i].first);
                }
                pos += numKeys; // Skip weights
            }
            
            if (mTokens[pos].first == "AGGREGATE") {
                if (cmdNum < pos + 1) {
                    throw EInvalidArgument();
                }
                
                pos += 1; // Skip "AGGREGATE"
                aggregate = mTokens[pos].first;
                if (aggregate != "SUM" && aggregate != "MIN" && aggregate != "MAX") {
                    throw EInvalidArgument();
                }
            }
        }
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        int numAdded = 0;
        
        switch (mCmdType)
        {
            case UNION:
            {
                if (!setMap.keyExists(dest)) {
                    FIRE_EVENT(EventPublisher::Event::del, dest);
                }
                
                numAdded = setMap.uni(dest, keys, weights, aggregate);
                
                FIRE_EVENT(EventPublisher::Event::zunionstore, dest);
                break;
            }
                
            case INTERSECT:
            {
                if (!setMap.keyExists(dest)) {
                    FIRE_EVENT(EventPublisher::Event::del, dest);
                }
                
                numAdded = setMap.intersect(dest, keys, weights, aggregate);
                
                FIRE_EVENT(EventPublisher::Event::zinterstore, dest);
                break;
            }
        }
        
        return CommandResultPtr(new CommandResult(to_string(numAdded),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr ZRangeByCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        int offset = 0;
        int count = INT_MAX;
        
        bool bWithScores = false;
        
        int pos = 4;
        
        if (cmdNum > pos && mTokens[4].first == "WITHSCORES") {
            bWithScores = true;
            pos++; // Skip WITHSCORES
        }
        
        if (cmdNum > pos && mTokens[pos].first == "LIMIT") {
            if (cmdNum != pos + 3) {
                throw EInvalidArgument();
            }
            
            offset = Utils::convertToInt(mTokens[pos + 1].first);
            count = Utils::convertToInt(mTokens[pos + 2].first);
        }
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        switch (mCmdType)
        {
            case RANGEBYSCORE:
            {
                double min = Utils::convertToDoubleByLimit(mTokens[2].first, false);
                double max = Utils::convertToDoubleByLimit(mTokens[3].first, true);
                
                SortedSetMap::RedisArray arr = setMap.rangeByScore(key,
                                                                   min, max,
                                                                   offset, count,
                                                                   bWithScores,
                                                                   false);
                return CommandResultPtr(new CommandResult(arr));
            }
                
            case REVRANGEBYSCORE:
            {
                double max = Utils::convertToDoubleByLimit(mTokens[2].first, true);
                double min = Utils::convertToDoubleByLimit(mTokens[3].first, false);
                
                SortedSetMap::RedisArray arr = setMap.rangeByScore(key,
                                                                   min, max,
                                                                   offset, count,
                                                                   bWithScores,
                                                                   true);
                return CommandResultPtr(new CommandResult(arr));
            }
                
        }
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr ZRangeByLexCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        int offset = 0;
        int count = INT_MAX;
        
        int pos = 4;
        
        if (cmdNum > pos && mTokens[pos].first == "LIMIT") {
            if (cmdNum != pos + 3) {
                throw EInvalidArgument();
            }
            
            offset = Utils::convertToInt(mTokens[pos + 1].first);
            count = Utils::convertToInt(mTokens[pos + 2].first);
        }
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        switch (mCmdType)
        {
            case RANGEBYLEX:
            {
                string min = Utils::convertToStringByLimit(mTokens[2].first, false);
                string max = Utils::convertToStringByLimit(mTokens[3].first, true);
                
                SortedSetMap::RedisArray arr = setMap.rangeByLex(key,
                                                                   min, max,
                                                                   offset, count,
                                                                   false);
                return CommandResultPtr(new CommandResult(arr));
            }
                
            case REVRANGEBYLEX:
            {
                string max = Utils::convertToStringByLimit(mTokens[2].first, false);
                string min = Utils::convertToStringByLimit(mTokens[3].first, true);
                
                SortedSetMap::RedisArray arr = setMap.rangeByLex(key,
                                                                   min, max,
                                                                   offset, count,
                                                                   true);
                return CommandResultPtr(new CommandResult(arr));
            }
                
        }
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr ZRemByCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& key = mTokens[1].first;
        int numRemoved = 0;
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        switch (mCmdType)
        {
            case REMRANGEBYSCORE:
            {
                double min = Utils::convertToDoubleByLimit(mTokens[2].first, false);
                double max = Utils::convertToDoubleByLimit(mTokens[3].first, true);
                
                numRemoved = setMap.remByScore(key, min, max);
                
                if (!setMap.keyExists(key)) {
                    FIRE_EVENT(EventPublisher::Event::del, key);
                }
                FIRE_EVENT(EventPublisher::Event::zrembyscore, key);
                
                break;
            }
                
            case REMRANGEBYRANK:
            {
                int start = Utils::convertToInt(mTokens[2].first);
                int stop = Utils::convertToInt(mTokens[3].first);
                
                Utils::normalizeIndexes(start, stop, setMap.size(key));
                
                numRemoved = setMap.remByRank(key, start, stop);
                
                if (!setMap.keyExists(key)) {
                    FIRE_EVENT(EventPublisher::Event::del, key);
                }
                FIRE_EVENT(EventPublisher::Event::zrembyrank, key);
                
                break;
            }
                
            case REMRANGEBYLEX:
            {
                string min = Utils::convertToStringByLimit(mTokens[2].first, false);
                string max = Utils::convertToStringByLimit(mTokens[3].first, true);
                
                numRemoved = setMap.remByLex(key, min, max);
                break;
            }
        }
        
        return CommandResultPtr(new CommandResult(to_string(numRemoved),
                                                  RedisProtocol::INTEGER));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}
