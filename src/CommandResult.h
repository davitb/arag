#ifndef __arag__CommandResult__
#define __arag__CommandResult__

#include <string>
#include <vector>

namespace arag
{

class CommandResult;
typedef std::shared_ptr<CommandResult> CommandResultPtr;
    
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
        SINGLE_RESPONSE,
        ARRAY_RESPONSE,
        MULTI_ARRAY_RESPONSE,
        MULTI_RESPONSE
    };
    
    CommandResult(const std::string& res, const int single);
    
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
    
    void appendToArray(const ResultArray& resArr);
    
    void appendToMultiArray(const CommandResultPtr& cmd);
    
    std::string toRedisResponse();
    
    bool isEmpty() const;
    
    Type getType() const;
    
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
