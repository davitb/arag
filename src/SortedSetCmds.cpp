#include "stdlib.h"
#include <algorithm>
#include "SortedSetCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

string ZAddCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        if (mTokens.size() % 2 != 0) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        int numAdded = 0;
        
        for (int i = 2; i < mTokens.size(); i += 2) {
            numAdded += setMap.insert(key, mTokens[i + 1].first, Utils::convertToDouble(mTokens[i].first));
        }
        
        return RedisProtocol::serializeNonArray(to_string(numAdded), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string ZRangeCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int start = Utils::convertToInt(mTokens[2].first);
        int end = Utils::convertToInt(mTokens[3].first);
        bool bWithScores = false;
        
        if (cmdNum == Consts::MAX_ARG_NUM) {
            if (mTokens[4].first != "WITHSCORES") {
                throw invalid_argument("Invalid args");
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
        
        return RedisProtocol::serializeArray(ret);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string ZScoreCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string member = mTokens[2].first;
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        switch (mCmdType)
        {
            case SCORE:
            {
                string score = Utils::dbl2str(setMap.score(key, member));
                return RedisProtocol::serializeNonArray(score, RedisProtocol::DataType::BULK_STRING);
            }
                
            case RANK:
            {
                int rank = setMap.rank(key, member, false);
                return RedisProtocol::serializeNonArray(to_string(rank), RedisProtocol::DataType::INTEGER);
            }
                
            case REVRANK:
            {
                int rank = setMap.rank(key, member, true);
                return RedisProtocol::serializeNonArray(to_string(rank), RedisProtocol::DataType::INTEGER);
            }
        }
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string ZCountCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;

        SortedSetMap& setMap = data.getSortedSetMap();
        
        switch (mCmdType)
        {
            case COUNT:
            {
                if (cmdNum != Consts::MAX_ARG_NUM) {
                    throw invalid_argument("Invalid args");
                }
                
                double min = Utils::convertToDoubleByLimit(mTokens[2].first, false);
                double max = Utils::convertToDoubleByLimit(mTokens[3].first, true);
                
                int count = setMap.count(key, min, max);
                
                return RedisProtocol::serializeNonArray(to_string(count), RedisProtocol::DataType::INTEGER);
            }
                
            case LEXCOUNT:
            {
                if (cmdNum != Consts::MAX_ARG_NUM) {
                    throw invalid_argument("Invalid args");
                }
                
                string min = Utils::convertToStringByLimit(mTokens[2].first, false);
                string max = Utils::convertToStringByLimit(mTokens[3].first, true);
                
                int count = setMap.lexCount(key, min, max);
                
                return RedisProtocol::serializeNonArray(to_string(count), RedisProtocol::DataType::INTEGER);
            }
                
            case CARD:
            {
                int size = setMap.size(key);
                return RedisProtocol::serializeNonArray(to_string(size), RedisProtocol::DataType::INTEGER);
            }
        }
        
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string ZIncrByCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        double by = Utils::convertToDouble(mTokens[2].first);
        string member = mTokens[3].first;
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        string ret = Utils::dbl2str(setMap.incrBy(key, member, by));
        
        return RedisProtocol::serializeNonArray(ret, RedisProtocol::DataType::BULK_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string ZRemCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        switch (mCmdType)
        {
            case REM:
            {
                if (cmdNum % 2 != 1) {
                    throw invalid_argument("Invalid args");
                }

                int numRemoved = 0;
                
                for (int i = 2; i < mTokens.size(); i += 2) {
                    numRemoved += setMap.rem(key, mTokens[i].first);
                }
                
                return RedisProtocol::serializeNonArray(to_string(numRemoved), RedisProtocol::DataType::INTEGER);
            }
                
            default:
            {
                throw invalid_argument("not supported");
            }
        }
        
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string ZUnionCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string dest = mTokens[1].first;
        int numKeys = Utils::convertToInt(mTokens[2].first);

        if (numKeys == 0) {
            throw invalid_argument("Invalid args");
        }
        
        int pos = 3;
        if (cmdNum < numKeys + pos) {
            throw invalid_argument("Invalid args");
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
                throw invalid_argument("Invalid args");
            }
            
            if (mTokens[pos].first == "WEIGHTS") {
                if (cmdNum < numKeys + pos) {
                    throw invalid_argument("Invalid args");
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
                    throw invalid_argument("Invalid args");
                }
                
                pos += 1; // Skip "AGGREGATE"
                aggregate = mTokens[pos].first;
                if (aggregate != "SUM" && aggregate != "MIN" && aggregate != "MAX") {
                    throw invalid_argument("Invalid args");
                }
            }
        }
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        int numAdded = 0;
        
        switch (mCmdType)
        {
            case UNION:
            {
                numAdded = setMap.uni(dest, keys, weights, aggregate);
                break;
            }
                
            case INTERSECT:
            {
                numAdded = setMap.intersect(dest, keys, weights, aggregate);
                break;
            }
        }
        
        return RedisProtocol::serializeNonArray(to_string(numAdded), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string ZRangeByCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
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
                throw invalid_argument("Invalid args");
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
                return RedisProtocol::serializeArray(arr);
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
                return RedisProtocol::serializeArray(arr);
            }
                
        }
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string ZRangeByLexCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int offset = 0;
        int count = INT_MAX;
        
        int pos = 4;
        
        if (cmdNum > pos && mTokens[pos].first == "LIMIT") {
            if (cmdNum != pos + 3) {
                throw invalid_argument("Invalid args");
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
                return RedisProtocol::serializeArray(arr);
            }
                
            case REVRANGEBYLEX:
            {
                string max = Utils::convertToStringByLimit(mTokens[2].first, false);
                string min = Utils::convertToStringByLimit(mTokens[3].first, true);
                
                SortedSetMap::RedisArray arr = setMap.rangeByLex(key,
                                                                   min, max,
                                                                   offset, count,
                                                                   true);
                return RedisProtocol::serializeArray(arr);
            }
                
        }
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string ZRemByCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        int numRemoved = 0;
        
        SortedSetMap& setMap = data.getSortedSetMap();
        
        switch (mCmdType)
        {
            case REMRANGEBYSCORE:
            {
                double min = Utils::convertToDoubleByLimit(mTokens[2].first, false);
                double max = Utils::convertToDoubleByLimit(mTokens[3].first, true);
                
                numRemoved = setMap.remByScore(key, min, max);
                break;
            }
                
            case REMRANGEBYRANK:
            {
                int start = Utils::convertToInt(mTokens[2].first);
                int stop = Utils::convertToInt(mTokens[3].first);
                
                Utils::normalizeIndexes(start, stop, setMap.size(key));
                
                numRemoved = setMap.remByRank(key, start, stop);
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
        
        return RedisProtocol::serializeNonArray(to_string(numRemoved), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}
