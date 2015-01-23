#ifndef __arag__Config__
#define __arag__Config__

#include <string>

namespace arag
{

static const std::string ARAG_VERSION = "0.1";
    
/*
    This class represents arag configuration data, read from config file.
*/
class Config
{
public:
    enum
    {
        DATABASE_COUNT = 16
    };
};
    
};

#endif /* defined(__arag__Config__) */
