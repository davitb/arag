#include "CommandResult.h"
#include "RedisProtocol.h"

using namespace arag;
using namespace std;

//----------------------------------------------------------------------------

CommandResult::CommandResult(const std::string& res, const int single)
{
    mSingleResult.first = res;
    mSingleResult.second = single;
    mType = SINGLE_RESPONSE;
}

CommandResult::CommandResult(const ResultArray& arr)
{
    mArrayResult = arr;
    mType = ARRAY_RESPONSE;
}

CommandResult::CommandResult(Type type)
{
    mType = type;
}

std::string CommandResult::toRedisResponse()
{
    if (mType == SINGLE_RESPONSE) {
        return RedisProtocol::serializeNonArray(mSingleResult.first,
                                                (RedisProtocol::DataType)mSingleResult.second);
    }
    if (mType == ARRAY_RESPONSE) {
        return RedisProtocol::serializeArray(mArrayResult);
    }
    if (mType == MULTI_ARRAY_RESPONSE) {
        stringstream str;
        for (int i = 0; i < mMultiArray.size(); ++i) {
            str << RedisProtocol::serializeArray(mMultiArray[i]);
        }
        return str.str();
    }
    if (mType == MULTI_RESPONSE) {
        stringstream str;
        for (int i = 0; i < mMultiResult.size(); ++i) {
            str << mMultiResult[i]->toRedisResponse();
        }
        return RedisProtocol::serializeArrayWithPreparedItems(size(), str.str());
    }

    // NO_RESPONSE
    return "";
}

int CommandResult::size()
{
    if (mType == SINGLE_RESPONSE) {
        return 1;
    }
    if (mType == ARRAY_RESPONSE) {
        return (int)mArrayResult.size();
    }
    if (mType == MULTI_ARRAY_RESPONSE) {
        return (int)mMultiArray.size();
    }
    if (mType == MULTI_RESPONSE) {
        return (int)mMultiResult.size();
    }
    
    // NO_RESPONSE
    return 0;
}

bool CommandResult::isEmpty() const
{
    return mType == NO_RESPONSE;
}

CommandResult::Type CommandResult::getType() const
{
    return mType;
}

void CommandResult::appendToArray(const ResultArray& resArr)
{
    mMultiArray.push_back(resArr);
}

void CommandResult::appendToMultiArray(const CommandResultPtr& cmd)
{
    mMultiResult.push_back(cmd);
}

CommandResultPtr CommandResult::redisNULLResult()
{
    return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING,
                                              RedisProtocol::NILL));
}

CommandResultPtr CommandResult::redisOKResult()
{
    return CommandResultPtr(new CommandResult("OK",
                                              RedisProtocol::SIMPLE_STRING));
}

CommandResultPtr CommandResult::redisErrorResult(const std::string& err)
{
    return CommandResultPtr(new CommandResult(err, RedisProtocol::ERROR));
}
