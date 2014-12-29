#ifndef __arag__ListMap__
#define __arag__ListMap__

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <mutex>
#include "SelfTest.h"

namespace arag
{

class ListMap
{
public:
    
private:
    std::unordered_map<std::string, std::list<std::string>> mList;
};

};

#endif /* defined(__arag__ListMap__) */
