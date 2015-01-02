#include "stdlib.h"
#include <algorithm>
#include "SortedSetCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

string ZAddCommand::execute(InMemoryData& data)
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

string ZRangeCommand::execute(InMemoryData& data)
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

string ZScoreCommand::execute(InMemoryData& data)
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

string ZCountCommand::execute(InMemoryData& data)
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
                
                double min = Utils::convertToDouble(mTokens[2].first);
                double max = Utils::convertToDouble(mTokens[3].first);
                
                int count = setMap.count(key, min, max);
                
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

string ZIncrByCommand::execute(InMemoryData& data)
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

string ZRemCommand::execute(InMemoryData& data)
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
