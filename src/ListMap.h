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

/*
    Implements a map which maps keys into Lists.
 */
class ListMap : public IMapCommon
{
public:

    enum Position
    {
        FRONT,
        BACK
    };
    
    // Adds a new value front or back
    int push(const std::string& key, const std::string& val, Position direction);

    // Returns the size of lists stored under key
    int size(const std::string& key);

    // Returns value of the element in position "pos" for "key"
    std::string val(const std::string& key, int pos);

    // Sets the value of the element in position "pos" for "key"
    void setVal(const std::string& key, int pos, const std::string& val);

    // Pops the front or back element from list under key
    std::string pop(const std::string &key, Position pos);

    // Removes "count" occurences of the given val.
    // If count is negative it removed from end.
    int rem(const std::string &key, const std::string &val, int count);

    // Trims the list for the given range
    void trim(const std::string &key, int start, int stop);

    // Finds the element with value equals to pivot and inserts new value
    // either after or before it.
    int insert(const std::string &key,
               const std::string &pos,
               const std::string &pivot,
               const std::string &val);

    // Returns the given range of elements
    std::vector<std::pair<std::string, int>> getRange(const std::string& key, int start, int end);
    
    // Returns number of all keys stored in the databases
    virtual int size();
    
    // Flushes all data
    virtual void flush();
    
    // Finds where the given key is and deletes its content
    virtual int delKey(const std::string& key);
    
    // Finds where the given key is and deletes its content
    virtual bool keyExists(const std::string& key);
    
    // Returns the container type associated with key
    IMapCommon::ContainerType getContainerType();
    
    // Renames the key to newkey
    virtual int rename(const std::string& key, const std::string& newKey);
    
private:
    typedef std::list<std::string> ListType;
    typedef std::unordered_map<std::string, ListType> ListMapType;
    ListMapType mListMap;
};

};

#endif /* defined(__arag__ListMap__) */
