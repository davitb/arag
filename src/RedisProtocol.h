#ifndef __arag__RedisProtocol__
#define __arag__RedisProtocol__

#include <string>
#include <vector>
#include "Commands.h"

namespace cache_server
{

namespace redis_const
{
    static std::string CRLF = "\r\n";
    static std::string NULL_BULK_STRING = "$-1\r\n";
    static std::string ERR_GENERIC = "-ERR unknown error\r\n";
};
    
class RedisProtocol
{
public:
  
    enum DataType
    {
        SIMPLE_STRING,
        ERROR,
        INTEGER,
        BULK_STRING,
        ARRAY,
        NILL
    };
  
    static std::vector<std::pair<std::string, int>> parse(const std::string& request);
    
    static std::string serializeNonArray(const std::string& response, const DataType type);
    
    static std::string serializeArray(const std::vector<std::pair<std::string, int>>& response);
};

};

#endif /* defined(__arag__RedisProtocol__) */
