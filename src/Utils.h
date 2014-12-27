#ifndef __arag__Utils__
#define __arag__Utils__

#include <vector>
#include <string>

namespace cache_server
{
    
class Utils
{
public:
    
    static void printVector(const std::vector<std::string>& vec);
    
    static int convertToInt(std::string val);
    
};

};

#endif /* defined(__arag__Utils__) */
