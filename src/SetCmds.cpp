#include "stdlib.h"
#include <algorithm>
#include "SetCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

string SAddCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        SetMap& setMap = data.getSetMap();
        
        int numAdded = 0;
        
        for (int i = 2; i < mTokens.size(); ++i) {
            numAdded += setMap.add(key, mTokens[i].first);
        }
        
        return RedisProtocol::serializeNonArray(to_string(numAdded), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string SMembersCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        SetMap& setMap = data.getSetMap();
        
        return RedisProtocol::serializeArray(setMap.getMembers(key));
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string SCardCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
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
                    throw invalid_argument("Invalid args");
                }
                
                string member = mTokens[2].first;
                ret = setMap.isMember(key, member);
                break;
            }
        }
        
        return RedisProtocol::serializeNonArray(to_string(ret), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string SRemCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        
        SetMap& setMap = data.getSetMap();
        
        int ret = 0;
        
        switch (mCmdType)
        {
            case REM:
            {
                if (cmdNum < Consts::MIN_ARG_NUM + 1) {
                    throw invalid_argument("Invalid args");
                }
                
                int numRemoved = 0;
                
                for (int i = 2; i < mTokens.size(); ++i) {
                    numRemoved += setMap.rem(key, mTokens[i].first);
                }
                
                return RedisProtocol::serializeNonArray(to_string(numRemoved),
                                                        RedisProtocol::DataType::INTEGER);
            }
                
            case POP:
            {
                string randMember = setMap.getRandMember(key);
                setMap.rem(key, randMember);
                return RedisProtocol::serializeNonArray(randMember, RedisProtocol::DataType::BULK_STRING);
            }
        }
        
        return RedisProtocol::serializeNonArray(to_string(ret), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string SDiffCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        SetMap& setMap = data.getSetMap();
        
        int ret = 0;
        
        switch (mCmdType)
        {
            case DIFF:
            {
                string key = mTokens[1].first;
                
                vector<string> diffKeys;
                for (int i = 2; i < mTokens.size(); ++i) {
                    diffKeys.push_back(mTokens[i].first);
                }
                
                SetMap::SetType newSet;
                setMap.diff(key, diffKeys, newSet);
                
                vector<std::pair<std::string, int>> retArray;
                
                for (auto val : newSet) {
                    retArray.push_back(make_pair(val, RedisProtocol::DataType::BULK_STRING));
                }
                
                return RedisProtocol::serializeArray(retArray);
            }
                
            case DIFFSTORE:
            {
                if (cmdNum < Consts::MIN_ARG_NUM + 1) {
                    throw invalid_argument("Invalid args");
                }
                
                string destKey = mTokens[1].first;
                string sourceKey = mTokens[2].first;
                
                vector<string> diffKeys;
                for (int i = 3; i < mTokens.size(); ++i) {
                    diffKeys.push_back(mTokens[i].first);
                }
                
                int num = setMap.diff(destKey, sourceKey, diffKeys);
                
                return RedisProtocol::serializeNonArray(to_string(num), RedisProtocol::DataType::INTEGER);
            }
        }
        
        return RedisProtocol::serializeNonArray(to_string(ret), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string SInterCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
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
                    retArray.push_back(make_pair(val, RedisProtocol::DataType::BULK_STRING));
                }
                
                return RedisProtocol::serializeArray(retArray);
            }
                
            case INTERSTORE:
            {
                if (cmdNum < Consts::MIN_ARG_NUM + 1) {
                    throw invalid_argument("Invalid args");
                }
                
                string destKey = mTokens[1].first;
                
                vector<string> keys;
                for (int i = 2; i < mTokens.size(); ++i) {
                    keys.push_back(mTokens[i].first);
                }
                
                int num = setMap.inter(destKey, keys);
                
                return RedisProtocol::serializeNonArray(to_string(num), RedisProtocol::DataType::INTEGER);
            }
        }
        
        return RedisProtocol::serializeNonArray(to_string(ret), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string SUnionCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
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
                    retArray.push_back(make_pair(val, RedisProtocol::DataType::BULK_STRING));
                }
                
                return RedisProtocol::serializeArray(retArray);
            }
                
            case UNIONSTORE:
            {
                if (cmdNum < Consts::MIN_ARG_NUM + 1) {
                    throw invalid_argument("Invalid args");
                }
                
                string destKey = mTokens[1].first;
                
                vector<string> keys;
                for (int i = 2; i < mTokens.size(); ++i) {
                    keys.push_back(mTokens[i].first);
                }
                
                int num = setMap.uni(destKey, keys);
                
                return RedisProtocol::serializeNonArray(to_string(num), RedisProtocol::DataType::INTEGER);
            }
        }
        
        return RedisProtocol::serializeNonArray(to_string(ret), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string SMoveCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        SetMap& setMap = data.getSetMap();
        
        string source = mTokens[1].first;
        string dest = mTokens[2].first;
        string member = mTokens[3].first;
        
        int ret = setMap.move(source, dest, member);
        
        return RedisProtocol::serializeNonArray(to_string(ret), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string SRandMemberCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        SetMap& setMap = data.getSetMap();
        
        string key = mTokens[1].first;
        
        if (cmdNum == Consts::MIN_ARG_NUM) {
            string member = setMap.getRandMember(key);
            return RedisProtocol::serializeNonArray(member, RedisProtocol::DataType::BULK_STRING);
        }
        
        int count = Utils::convertToInt(mTokens[2].first);
        
        vector<pair<string, int>> res;
        if (count < 0) {
            for (int i = 1; i <= abs(count); ++i) {
                res.push_back(make_pair(setMap.getRandMember(key), RedisProtocol::DataType::BULK_STRING));
            }
        }
        else {
            res = setMap.getRandMembers(key, count);
        }
        
        return RedisProtocol::serializeArray(res);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}
