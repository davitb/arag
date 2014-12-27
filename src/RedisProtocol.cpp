#include <iostream>
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::redis_const;

// Convert data type symbol into DataType
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

// Parse str, extract string value from it and return its dataType and end position
string parseData(const string& str, size_t pos, size_t& newPos, RedisProtocol::DataType& type)
{
    char ch = str[pos];

    pos += 1; // skip data type symbol ('$', ':', ...)
    size_t ind = pos;
    int num = -1;
    
    // If it's a BULK_STRING - extract the number of bytes
    if (ch == '$') {
        size_t ind = str.find(CRLF, pos);
        if (ind == std::string::npos) {
            throw invalid_argument("parseArray: invalid array");
        }
        
        // We will extract number of bytes but will ignore it for now
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
    
    // Example of Redis formatted stings
    // "+test\r\n"
    // "$3\r\nfoo\r\n"
    // "*2\r\n$3\r\nfoo\r\n$3\r\nbar\r\n"
    
    // We expect request to always be ARRAY (start with '*')
    if (request[0] != '*' || len < 4) {
        throw invalid_argument("parseArray: invalid array");
    }
    
    size_t ind = 0;
    DataType type;

    // Get number of elements in ARRAY
    string data = parseData(request, 0, ind, type);
    int num = Utils::convertToInt(data);
    
    vector<pair<string, int>> tokens(num);

    for (int i = 0; i < num; ++i) {
        size_t newPos = 0;
        // Extract next value
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
