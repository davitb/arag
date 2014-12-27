#include <iostream>
#include <sstream>
#include "Utils.h"
#include "RedisProtocol.h"

using namespace std;
using namespace arag;

void Utils::printVector(const std::vector<std::string>& vec)
{
    cout << "vector size: " << vec.size() << endl;
    for (auto str : vec) {
        cout << str << " ";
    }
    cout << endl;
}

int Utils::convertToInt(std::string val)
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

int Utils::countSetBits(unsigned char n)
{
    unsigned int count = 0;
    while (n)
    {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

string Utils::performBitOperation(const string& op, const string& key1, const string& key2)
{
    string val;
    size_t key1Len = key1.length();
    size_t key2Len = key2.length();
    size_t maxLen = max(key1.length(), key2.length());
    stringstream ss;
    
    if (op != "NOT") {
        
        function<char(const string&, char, char)> bitFunc = [](const string& op, char ch1, char ch2) {
            if (op == "AND") {
                return ch1 & ch2;
            }
            if (op == "OR") {
                return ch1 | ch2;
            }
            return ch1 ^ ch2;
        };
        
        for (int i = 0; i < maxLen; ++i) {
            if (i >= key2Len) {
                ss << bitFunc(op, key1[i], 0);
            }
            else
            if (i >= key1Len) {
                ss << bitFunc(op, key2[i], 0);
            }
            else {
                ss << bitFunc(op, key1[i], key2[i]);
            }
        }
    }
    else {
        for (int i = 0; i < key1Len; ++i) {
            ss << ~key1[i];
        }
    }

    ss >> val;
    return val;
}

string Utils::performBitOperation(const string& op, const vector<pair<string, int>>& keys)
{
    string final = keys[0].first;
    
    if (keys.size() == 1) {
        return performBitOperation(op, keys[0].first, keys[0].first);
    }
    
    for (int i = 1; i < keys.size(); ++i) {
        final = performBitOperation(op, final, keys[i].first);
    }
    
    return final;
}
