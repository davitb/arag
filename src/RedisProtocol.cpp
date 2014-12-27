#include <iostream>
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace cache_server;
using namespace cache_server::redis_const;

RedisProtocol::DataType getDataType(char ch)
{
    switch (ch)
    {
        case '+':
            return RedisProtocol::DataType::SIMPLE_STRING;
            
        case '$':
            return RedisProtocol::DataType::BULK_STRING;
            
        case ':':
            return RedisProtocol::DataType::INTEGER;

        case '-':
            return RedisProtocol::DataType::ERROR;
            
        case '*':
            return RedisProtocol::DataType::ARRAY;
    }
    
    throw invalid_argument("RedisProtocol::serialize: Unknown type");
}

string parseData(const string& str, size_t pos, size_t& newPos, RedisProtocol::DataType& type)
{
    char ch = str[pos];

    pos += 1; // skip data type symbol ('$', ':', ...)
    size_t ind = pos;
    int num = -1;
    
    if (ch == '$') {
        size_t ind = str.find(CRLF, pos);
        if (ind == std::string::npos) {
            throw invalid_argument("parseArray: invalid array");
        }
        
        num = Utils::convertToInt(str.substr(pos, ind - pos));
        
        ind += CRLF.length(); // skip CRLF
        pos = ind;
    }

    ind = str.find(CRLF, pos);
    if (ind == std::string::npos) {
        throw invalid_argument("parseArray: invalid array");
    }

    // Make sure that bulk array indicated the right size
    if (num != -1 && num != ind - pos) {
        throw invalid_argument("parseArray: invalid array");
    }
    
    type = getDataType(ch);
    newPos = ind + CRLF.length(); // skip CRLF
    return str.substr(pos, ind - pos);
}

vector<pair<string, int>> RedisProtocol::parse(const std::string& request)
{
    size_t len = request.length();
    
    // Example of an array
    // "+davit\r\n"
    // "$3\r\nfoo\r\n"
    // "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n"
    
    if (request[0] != '*' || len < 4) {
        throw invalid_argument("parseArray: invalid array");
    }
    
    size_t ind = 0;
    DataType type;
    string data = parseData(request, 0, ind, type);

    // Get number of elements in ARRAY
    int num = Utils::convertToInt(data);
    
    vector<pair<string, int>> tokens(num);

    for (int i = 0; i < num; ++i) {
        size_t newPos = 0;
        data = parseData(request, ind, newPos, type);
        if (newPos > len) {
            throw invalid_argument("RedisProtocol::parse: Invalid string");
        }
        tokens[i] = make_pair(data, type);
        ind = newPos;
    }
    
    if (ind != len) {
        throw invalid_argument("RedisProtocol::parse: Invalid string");
    }
    
    return tokens;
}

string RedisProtocol::serializeNonArray(const string& response, const DataType type)
{
    string out;
    
    switch (type)
    {
        case SIMPLE_STRING:
            out = "+" + response + CRLF;
            break;
            
        case BULK_STRING:
            out = "$" + to_string(response.length()) + CRLF;
            out += response + CRLF;
            break;

        case INTEGER:
            out = ":" + response + CRLF;
            break;

        case ERROR:
            out = "-" + response + "\r\n";
            break;

        case NILL:
            out = NULL_BULK_STRING;
            break;
            
        default:
            throw invalid_argument("RedisProtocol::serialize: Unknown type");
    }
    
    return out;
}

string RedisProtocol::serializeArray(const vector<pair<string, int>>& response)
{
    string out;
    
    out = "*" + to_string(response.size()) + CRLF;
    for (auto resp : response) {
        out += serializeNonArray(resp.first, (DataType)resp.second);
    }
    
    return out;
}
