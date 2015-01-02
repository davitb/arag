#ifndef __arag__SortedSetMap__
#define __arag__SortedSetMap__

#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <functional>
#include "random_access_skip_list.h"

namespace arag
{

class SortedSetMap
{
private:
    
    class Item
    {
    public:
        
        std::string val;
        double score;
        
        Item(double s) { score = s; }
        
        Item(std::string v, double s) {
            val = v;
            score = s;
        }
        
        struct Less {
            bool operator()(const Item &lhs, const Item &rhs) const
            {
                if (lhs.score < rhs.score) {
                    return true;
                }
                if (lhs.score == rhs.score && lhs.val.compare(rhs.val) < 0) {
                    return true;
                }
                if (lhs.score == rhs.score && lhs.val == rhs.val) {
                    return false;
                }
                return false;
            }
        };
    };

public:
    class SortedSet
    {
    public:
        typedef goodliffe::random_access_skip_list<Item, Item::Less> SkipListType;
        typedef std::unordered_map<std::string, double> MapType;
        MapType mMap;
        SkipListType mSkipList;
    };
    
public:
    typedef std::vector<std::pair<std::string, int>> RedisArray;

    int insert(const std::string &key, const std::string& val, double score);
    
    RedisArray range(const std::string &key, int start, int end, bool bWithScores);
    
    RedisArray revRange(const std::string &key, int start, int end, bool bWithScores);
    
    double score(const std::string& key, const std::string& member);

    int rank(const std::string& key, const std::string& member, bool reverse);

    int count(const std::string &key, double min, double max);
    
    int size(const std::string &key);
    
    double incrBy(const std::string &key, const std::string &member, double by);
    
    int rem(const std::string &key, const std::string &member);
    
private:

    std::unordered_map<std::string, SortedSet> mSetMap;
};

};

#endif /* defined(__arag__SortedSetMap__) */
