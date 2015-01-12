#ifndef __arag__ListMap__
#define __arag__ListMap__

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <mutex>
#include "IMapCommon.h"
#include "SelfTest.h"

namespace arag
{

class ListMap : public IMapCommon
{
public:

    enum Position
    {
        FRONT,
        BACK
    };
    
    int push(const std::string& key, const std::string& val, Position direction);
    
    virtual int size();

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
    
    virtual void clearKeys();
    
    virtual int delKey(const std::string& key);
    
    virtual bool keyExists(const std::string &key);
    
private:
    typedef std::list<std::string> ListType;
    typedef std::unordered_map<std::string, ListType> ListMapType;
    ListMapType mListMap;
};

};

#endif /* defined(__arag__ListMap__) */
