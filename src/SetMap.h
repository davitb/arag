#ifndef __arag__SetMap__
#define __arag__SetMap__

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

namespace arag
{

class SetMap
{
public:
    typedef std::unordered_set<std::string> SetType;
    typedef std::unordered_map<std::string, SetType> SetMapType;
   
    int size(const std::string& key);
    
    int isMember(const std::string& key, const std::string& val);
    
    int add(const std::string& key, const std::string& val);
    
    std::vector<std::pair<std::string, int>> getMembers(const std::string& key);
    
    std::string getRandMember(const std::string& key);
    
    std::vector<std::pair<std::string, int>> getRandMembers(const std::string &key, const int n);
    
    int rem(const std::string& key, const std::string& val);
    
    void diff(const std::string& key, const std::vector<std::string>& diffKeys, SetType& destSet);

    int diff(const std::string& destKey, const std::string& key, const std::vector<std::string>& diffKeys);

    void inter(const std::vector<std::string>& keys, SetType& destSet);
    
    int inter(const std::string& destKey, const std::vector<std::string>& keys);

    void uni(const std::vector<std::string>& keys, SetType& destSet);
    
    int uni(const std::string& destKey, const std::vector<std::string>& keys);

    int move(const std::string& source, const std::string& dest, const std::string& member);
    
private:
    SetMapType mSetMap;
};

};

#endif /* defined(__arag__SetMap__) */
