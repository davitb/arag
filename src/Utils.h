#ifndef __arag__Utils__
#define __arag__Utils__

#include <vector>
#include <string>

namespace arag
{
    
class Utils
{
public:
    
    static void printVector(const std::vector<std::string>& vec);
    
    static int convertToInt(std::string val);

    static double convertToDouble(std::string val);
    
    static int countSetBits(unsigned char n);
    
    static std::string performBitOperation(const std::string& op,
                                           const std::vector<std::pair<std::string, int>>& keys);
    
    static std::string performBitOperation(const std::string& op,
                                           const std::string& key1,
                                           const std::string& key2);
    
    static int getBitPos(const std::string& str, int setBit, bool lookInRange);
    
    static int getBit(const std::string& str, int offset);
    
    static void setBit(std::string& str, int offset, int bit);
    
    static std::string dbl2str(double d);
};

};

#endif /* defined(__arag__Utils__) */
