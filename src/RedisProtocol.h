#ifndef __arag__RedisProtocol__
#define __arag__RedisProtocol__

#include <string>
#include <vector>
#include "Commands.h"

namespace arag
{

namespace redis_const
{
    static std::string CRLF = "\r\n";
    static std::string NULL_BULK_STRING = "$-1\r\n";
    static std::string ERR_GENERIC = "-ERR unknown error\r\n";
};

/*
    This class implements a parser for Redis Protocol (http://redis.io/topics/protocol)
 */
class RedisProtocol
{
public:
  
    // Data types supported by the protocol
    enum DataType
    {
        SIMPLE_STRING,
        ERROR,
        INTEGER,
        BULK_STRING,
        ARRAY,
        NILL
    };
  
    // Parses the request and returns a pair of {value, dataType}.
    // Redis server assumes that the request has always ARRAY type.
    static std::vector<std::pair<std::string, int>> parse(const std::string& request);

    // Serializes given response into Redis format.
    static std::string serializeNonArray(const std::string& response, const DataType type);

    // Serializes given vector of {value, dataType} into Redis ARRAY
    static std::string serializeArray(const std::vector<std::pair<std::string, int>>& response);
};

};

#endif /* defined(__arag__RedisProtocol__) */
