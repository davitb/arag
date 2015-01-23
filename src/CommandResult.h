#ifndef __arag__CommandResult__
#define __arag__CommandResult__

#include <string>
#include <vector>

namespace arag
{

class CommandResult;
typedef std::shared_ptr<CommandResult> CommandResultPtr;

/*
    This class represents the result of any arag command. It can be directly converted 
    to Redis data type.
*/
class CommandResult
{
public:
    typedef std::pair<std::string, int> ResultSingle;
    typedef std::vector<ResultSingle> ResultArray;
    typedef std::vector<ResultArray> ResultMultiArray;
    typedef std::vector<CommandResultPtr> ResultMultiCommand;
    
    enum Type
    {
        NO_RESPONSE,
        // Regular Redis basic type
        SINGLE_RESPONSE,
        // Regular Redis array
        ARRAY_RESPONSE,
        // This type is only used for PubSub. The different with MULTI_ARRAY is that
        // MULTI_ARRAY_RESPONSE is an array of any object while ARRAY_RESPONSE is a sequence
        // of arrays
        MULTI_ARRAY_RESPONSE,
        // Array of any object (can be array as well)
        MULTI_RESPONSE
    };
    
    // Create a Redis basic type
    CommandResult(const std::string& res, const int single);

    // Create a Redis array type.
    CommandResult(const ResultArray& mult);
    
    CommandResult(Type type);
    
    const ResultSingle& getSingleResult() const
    {
        return mSingleResult;
    }

    const ResultMultiCommand& getMultiCommandResult() const
    {
        return mMultiResult;
    }
    
    // Append the given array to MULTI_ARRAY_RESPONSE
    void appendToArray(const ResultArray& resArr);

    // Append the given command result to MULTI_RESPONSE
    void appendToMultiArray(const CommandResultPtr& cmd);
    
    // Convert to Redis response
    std::string toRedisResponse();
    
    // Checks if it contains any type or not (NO_RESPONSE)
    bool isEmpty() const;
    
    Type getType() const;
    
    // Returns the number of elements in the result
    int size();
    
    static CommandResultPtr redisNULLResult();
    
    static CommandResultPtr redisOKResult();

    static CommandResultPtr redisErrorResult(const std::string& err);
    
public:
    ResultSingle mSingleResult;
    ResultArray mArrayResult;
    ResultMultiArray mMultiArray;
    ResultMultiCommand mMultiResult;
    Type mType;
};

};

#endif /* defined(__arag__CommandResult__) */
