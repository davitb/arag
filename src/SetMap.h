#ifndef __arag__SetMap__
#define __arag__SetMap__

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include "IMapCommon.h"

namespace arag
{

/*
    Implements a map to store Key->Set mapping.
 */
class SetMap : public IMapCommon
{
public:
    typedef std::unordered_set<std::string> SetType;
    typedef std::unordered_map<std::string, SetType> SetMapType;

    // Returns the size of set stored under key
    int size(const std::string& key);

    // Returns 1 if val is member of the set
    int isMember(const std::string& key, const std::string& val);

    // Adds val to the set and returns 1 if addition was successful
    int add(const std::string& key, const std::string& val);

    // Returns all members of the set
    std::vector<std::pair<std::string, int>> getMembers(const std::string& key);

    // Returns a random element from the set
    std::string getRandMember(const std::string& key);

    // Returns n random elements from the set
    std::vector<std::pair<std::string, int>> getRandMembers(const std::string &key, const int n);

    // Removes val from the set
    int rem(const std::string& key, const std::string& val);

    // Calculates the difference between set[key] and set[diffKeys] and stores inside destSet
    void diff(const std::string& key, const std::vector<std::string>& diffKeys, SetType& destSet);

    // Calculates the difference between set[key] and set[diffKeys] and stores inside set[destKey]
    int diff(const std::string& destKey, const std::string& key, const std::vector<std::string>& diffKeys);

    // Calculates the intersection between set[keys] and stores in destSet
    void inter(const std::vector<std::string>& keys, SetType& destSet);

    // Calculates the intersection between set[keys] and stores in set[destKey]
    int inter(const std::string& destKey, const std::vector<std::string>& keys);

    // Calculates the union of set[keys] and stores in destSet
    void uni(const std::vector<std::string>& keys, SetType& destSet);

    // Calculates the union of set[keys] and stores in set[destKey]
    int uni(const std::string& destKey, const std::vector<std::string>& keys);

    // Moves member from set[source] to dest set[dest]
    int move(const std::string& source, const std::string& dest, const std::string& member);

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
    SetMapType mSetMap;
};

};

#endif /* defined(__arag__SetMap__) */
