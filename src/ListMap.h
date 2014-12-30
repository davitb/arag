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

    enum Position
    {
        FRONT,
        BACK
    };
    
    bool keyExists(std::string key);
    
    int push(const std::string& key, const std::string& val, Position direction);
    
    int len(const std::string& key);
    
    std::string val(const std::string& key, int pos);
    
    void setVal(const std::string& key, int pos, const std::string& val);
    
    std::string pop(const std::string &key, Position pos);
    
    int rem(const std::string &key, const std::string &val, int count);
    
    void trim(const std::string &key, int start, int stop);

    int insert(const std::string &key,
               const std::string &pos,
               const std::string &pivot,
               const std::string &val);
    
    std::vector<std::pair<std::string, int>> getRange(const std::string& key, int start, int end);
    
private:
    std::unordered_map<std::string, std::list<std::string>> mListMap;
};

};

#endif /* defined(__arag__ListMap__) */
