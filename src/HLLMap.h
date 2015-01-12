#ifndef __arag__HLLMap__
#define __arag__HLLMap__

#include <unordered_map>
#include <string>
#include <vector>
#include "hyperloglog.hpp"
#include "IMapCommon.h"

namespace arag
{
    
class HLLMap : public IMapCommon
{
private:

    enum
    {
        BIT_WIDTH = 10
    };
    
public:
    typedef std::unordered_map<std::string, hll::HyperLogLog> HLLMapType;
    
    int add(const std::string &key, const std::string& val);
    
    int count(const std::string &key);

    int count(const std::vector<std::string> &keys);
    
    void merge(const std::string& destKey, const std::vector<std::string> &keys);
    
    virtual int size();
    
    virtual int delKey(const std::string& key);
    
    virtual bool keyExists(const std::string& key);
    
    virtual void clearKeys();
    
private:
    
    HLLMapType mHLLMap;
};

};



#endif /* defined(__arag__HLLMap__) */
