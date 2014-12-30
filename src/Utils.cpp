#include <iostream>
#include <sstream>
#include "Utils.h"
#include "RedisProtocol.h"
#include <bitset>

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

double Utils::convertToDouble(std::string val)
{
    try {
        size_t idx = 0;
        cout << val << endl;
        double dval = std::stod(val, &idx);
        if (idx != val.length()) {
            throw invalid_argument("Must be a double number");
        }
        
        return dval;
    }
    catch (invalid_argument& e) {
    }
    
    throw invalid_argument("Must be double number");
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

int Utils::getBitPos(const std::string& str, int setBit, bool lookInRange)
{
    if (setBit == 1 && str == "") {
        return -1;
    }

    if (setBit == 0 && str == "") {
        return 0;
    }
    
    for (int i = 0; i < str.length(); ++i) {
        bitset<8> bits(str[i]);
        
        cout << bits << endl;
        for (int j = 0; j < 8; ++j) {
            if (bits[7 - j] == setBit) {
                return i * 8 + j;
            }
        }
    }

    if (!lookInRange && setBit == 0) {
        return (int)str.length() * 8;
    }

    return -1;
}

int Utils::getBit(const std::string& str, int offset)
{
    if (str.length() == 0 || offset >= (str.length() * 8)) {
        return 0;
    }

    char ch = str[offset / 8];
    int pos = 8 - offset % 8;
    
    return !!(ch & (1 << pos));
}

void Utils::setBit(std::string& str, int offset, int bit)
{
    char& ch = str[offset / 8];
    int pos = 8 - offset % 8;
    
    if (bit == 1) {
        ch |= (1 << pos);
    }
    else {
        ch &= ~(1 << pos);
    }
}

std::string Utils::dbl2str(double d)
{
    size_t len = std::snprintf(0, 0, "%.10f", d);
    std::string s(len+1, 0);
    // technically non-portable, see below
    std::snprintf(&s[0], len+1, "%.10f", d);
    // remove nul terminator
    s.pop_back();
    // remove trailing zeros
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);
    // remove trailing point
    if(s.back() == '.') {
        s.pop_back();
    }
    return s;
}

void Utils::normalizeIndexes(int &start, int &end, const int& len)
{
    if (start >= len) {
        start = len;
    }
    
    if (end >= len) {
        end = len - 1;
    }
    
    if (start < 0) {
        start = len - abs(start);
        if (start < 0) {
            start = 0;
        }
    }
    
    if (end < 0) {
        end = len - abs(end);
        if (end < 0) {
            end = 0;
        }
    }
}
