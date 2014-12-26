#ifndef __arag__RedisProtocol__
#define __arag__RedisProtocol__

#include <string>
#include <vector>
#include "Commands.h"

namespace cache_server
{
static std::string CRLF = "\r\n";
static std::string NULL_BULK_STRING = "$-1\r\n";
static std::string ERR_GENERIC = "-ERR unknown error\r\n";
    
class RedisProtocol
{
public:
  
    enum DataType
    {
        SIMPLE_STRING,
        ERROR,
        INTEGER,
        BULK_STRING,
        ARRAY
    };
    
    static int convertToInt(std::string val);
    
    static std::vector<std::string> parse(const std::string& request);
    
    static std::string serializeNonArray(const std::string& response, const DataType type);
    
    static std::string serializeArray(const std::vector<std::pair<std::string, bool>>& response);
};

};

#endif /* defined(__arag__RedisProtocol__) */
