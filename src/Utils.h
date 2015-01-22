#ifndef __arag__Utils__
#define __arag__Utils__

#include <vector>
#include <string>
#include <unordered_map>

namespace arag
{
    
class Utils
{
public:
    
    enum Format
    {
        HEX,
        BASE64_SAFE
    };
    
    static void printVector(const std::vector<std::string>& vec);
    
    // Converts the given string to int. Raises exception when cannot convert.
    static int convertToInt(const std::string& val);

    // Converts the given string to double. Raises exception when cannot convert.
    static double convertToDouble(const std::string& val);
    
    // Converts the given string to double.
    // Additionally understands left and right limit indicators as specified in Redis docs.
    static double convertToDoubleByLimit(std::string val, bool bUpperLimit);

    // Returns a string to the closest value by interpreting left and right limit
    // indicators as specified in Redis docs.
    static std::string convertToStringByLimit(std::string val, bool bUpperLimit);

    // Counts the number of bits in N
    static int countSetBits(unsigned char n);

    // Performs logical bit operation on given list of strings
    static std::string performBitOperation(const std::string& op,
                                           const std::vector<std::pair<std::string, int>>& keys);

    // Performs logical bit operation on given 2 strings
    static std::string performBitOperation(const std::string& op,
                                           const std::string& key1,
                                           const std::string& key2);
    
    // Returns position of the first occurrance of "setBit" in a string
    static int getBitPos(const std::string& str, int setBit, bool lookInRange);

    // Returns value of the bit in given offset
    static int getBit(const std::string& str, int offset);
    
    // Sets the value of the bit in given offset
    static void setBit(std::string& str, int offset, int bit);

    // Converts doulbe to string and removes redundant zeros at the end (if any)
    static std::string dbl2str(double d);

    // Normalizes indexes following Redis docs
    static void normalizeIndexes(int& start, int& end, const int& len);

    // Returns a random integer from [min, max]
    static int genRandom(int min, int max);

    // Returns time of day in seconds and miliseconds
    static void getTimeOfDay(long& secs, long& msecs);

    // Checks if the given string matches given pattern following the rules described
    // in Redis's PubSub docs
    static bool checkPubSubPattern(const std::string& str, std::string patt);

    // Calculates and returns SHA1 of a string
    static std::string sha1(const std::string& source, Format format = HEX);

    // Converts given buffer to HEX
    static std::string toHexString(unsigned char* data, unsigned int len);
};

};

#endif /* defined(__arag__Utils__) */
