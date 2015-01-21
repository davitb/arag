#include <iostream>
#include <sstream>
#include "Utils.h"
#include "RedisProtocol.h"
#include <bitset>
#include <random>
#include <cfloat>
#include <regex>
#include "sha1.h"

#include <sys/time.h>

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


double Utils::convertToDoubleByLimit(std::string val, bool bUpperLimit)
{
    if (val[0] == '(') {
        val = val.substr(1, val.length() - 1);
        // We will treat this number as the lowest possible double that our system supports.
        // Ideally we would use the predefined DBL_EPSILON from <cfloat> however it seems
        // that in order to properly use it we need to turn off optimization on compilers.
        // From other side, we don't really need to support such small numbers in our system.
        // Take a look at this discussion
        // http://stackoverflow.com/questions/7517588/different-floating-point-result-with-optimization-enabled-compiler-bug
        const double epslion = 2.2204460492503131E-10;
        if (bUpperLimit) {
            return Utils::convertToDouble(val) - epslion;
        }
        else {
            return Utils::convertToDouble(val) + epslion;
        }
    }
    if (val[0] == '[') {
        val = val.substr(1, val.length() - 1);
        return Utils::convertToDouble(val);
    }
    else
    if (val == "-inf") {
        return DBL_MIN;
    }
    else
    if (val == "+inf") {
        return DBL_MAX;
    }

    return Utils::convertToDouble(val);
}

string Utils::convertToStringByLimit(std::string val, bool bUpperLimit)
{
    if (val[0] == '(') {
        val = val.substr(1, val.length() - 1);
        const char epslion = char(1);
        if (bUpperLimit) {
            val[val.size() - 1] -= epslion;
            val.append(1, epslion);
        }
        else {
            val.append(1, epslion);
        }
    }
    if (val[0] == '[') {
        val = val.substr(1, val.length() - 1);
    }
    else
    if (val == "-") {
        val[0] = CHAR_MIN;
    }
    else
    if (val == "+") {
        val[0] = CHAR_MAX;
    }
    
    return val;
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

int Utils::genRandom(int min, int max)
{
    std::random_device rd;
    std::default_random_engine e1(rd());
    std::uniform_int_distribution<int> uniform_dist(min, max);
    return uniform_dist(e1);
}

void Utils::getTimeOfDay(long &secs, long &msecs)
{
    timeval t;
    gettimeofday(&t, NULL);
    
    secs = t.tv_sec;
    msecs = t.tv_usec;
}

bool Utils::checkPattern(const std::string& str, std::string patt)
{
    string finalStr = str;
    size_t pos = 0;
    
    while ((pos = patt.find('?')) != std::string::npos) {
        if (pos != 0 && patt[pos - 1] != '\\') {
            patt[pos] = '.';
        }
        pos++;
    }
    
    std::regex r(patt);
    
    return std::regex_search(str, r);
}

std::string Utils::sha1(const std::string& source, Format format)
{
    unsigned char hash[20];

    sha1::calc(source.c_str(), (int)source.length(), hash);
    
    return toHexString(hash, sizeof(hash));
}

std::string Utils::toHexString(unsigned char* input, unsigned int len)
{
    static const char* const lut = "0123456789ABCDEF";
    
    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}
