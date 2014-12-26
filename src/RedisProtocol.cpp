#include <iostream>
#include "RedisProtocol.h"
#include "Commands.h"

using namespace std;
using namespace cache_server;

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
    }
    
    throw invalid_argument("RedisProtocol::serialize: Unknown type");
}

int RedisProtocol::convertToInt(std::string val)
{
    try {
        size_t idx = 0;
        int intVal = std::stoi(val, &idx);
        if (idx != val.length()) {
            throw invalid_argument("Must be a number");
        }
        
        return intVal;
    }
    catch (invalid_argument& e) {
    }
    
    throw invalid_argument("Must be a number");
}



string parseData(const string& str, size_t pos, size_t& newPos)
{
    char ch = str[pos];

    pos += 1; // skip data type symbol ('$', ':', ...)
    size_t ind = pos;
    
    if (ch == '$') {
        size_t ind = str.find(CRLF, pos);
        if (ind == std::string::npos) {
            throw invalid_argument("parseArray: invalid array");
        }
        
        //num = RedisProtocol::convertToInt(str.substr(pos, ind - pos));
        
        ind += CRLF.length(); // skip CRLF
        pos = ind;
    }

    ind = str.find(CRLF, pos);
    if (ind == std::string::npos) {
        throw invalid_argument("parseArray: invalid array");
    }

    newPos = ind + CRLF.length(); // skip CRLF
    return str.substr(pos, ind - pos);
}

vector<string> RedisProtocol::parse(const std::string& request)
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
    string data = parseData(request, 0, ind);

    // Get number of elements in ARRAY
    int num = convertToInt(data);
    
    vector<string> tokens(num);

    for (int i = 0; i < num; ++i) {
        size_t newPos = 0;
        data = parseData(request, ind, newPos);
        if (newPos > len) {
            throw invalid_argument("RedisProtocol::parse: Invalid string");
        }
        tokens[i] = data;
        ind = newPos;
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
            
        default:
            throw invalid_argument("RedisProtocol::serialize: Unknown type");
    }
    
    return out;
}

string RedisProtocol::serializeArray(const vector<pair<string, bool>>& response)
{
    string out;
    
    if (response.size() == 0) {
        throw invalid_argument("RedisProtocol::serialize: Invalid response");
    }
    
    out = "*" + to_string(response.size()) + CRLF;
    for (auto resp : response) {
        if (resp.second) {
            out += NULL_BULK_STRING;
        }
        else {
            out += "$" + to_string(resp.first.length()) + CRLF + resp.first + CRLF;
        }
    }
    
    return out;
}
