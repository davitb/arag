#ifndef __arag__SortedSetMap__
#define __arag__SortedSetMap__

#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <functional>
#include "random_access_skip_list.h"
#include "IMapCommon.h"

namespace arag
{

/*
    This class implements a map which maps keys to SortedSet elements.
    SortedSet is a structures which keeps elements sorted, and
    allows O(1) access to its elements and O(1) addition of new elements.
    The underlying structure we use is skiplist.
 */
class SortedSetMap : public IMapCommon
{
public:
    
    /*
        Items contains a string value and associated score.
        All items are sorted with their scores.
    */
    class Item
    {
    public:
        
        std::string val;
        double score;
        
        Item(double s) { score = s; }
        
        Item(std::string v, double s) { val = v; score = s; }
        
        struct Less {
            // A special comparison function which helps keeping elements sorted.
            bool operator()(const Item &lhs, const Item &rhs) const;
        };
    };

    class SortedSet
    {
    public:
        typedef goodliffe::random_access_skip_list<Item, Item::Less> SkipListType;
        typedef std::unordered_map<std::string, double> MapType;
        // For each SortedSet object we keep a map and a skiplist. There is a duplication of
        // data but this representation simplifies things.
        MapType mMap;
        SkipListType mSkipList;
    };
    
public:
    typedef std::vector<std::pair<std::string, int>> RedisArray;
    
    enum Bound
    {
        UPPER_BOUND,
        LOWER_BOUND
    };

    // Inserts a new {value, score} item to SortedSet[key]
    int insert(const std::string &key, const std::string& val, double score);

    // Returns elements for the given range from SortedSet[key]
    RedisArray range(const std::string &key, int start, int end, bool bWithScores);

    // Returns "count" elements that have scores in range [min, max]
    RedisArray rangeByScore(const std::string &key,
                            double min,
                            double max,
                            int offset,
                            int count,
                            bool bWithScores,
                            bool bReverse);

    // Returns "count" elements that have values in range [min, max]
    RedisArray rangeByLex(const std::string &key,
                          const std::string& min,
                          const std::string& max,
                            int offset,
                            int count,
                            bool bReverse);

    // Returns the range in reverse order
    RedisArray revRange(const std::string &key, int start, int end, bool bWithScores);

    // Returns the score of a given member
    double score(const std::string& key, const std::string& member);

    // Returns the rank of a given member
    int rank(const std::string& key, const std::string& member, bool reverse);

    // Returns the number of elements with scores between [min, max]
    int count(const std::string &key, double min, double max);
    
    // Returns the number of elements with values between [min, max]
    int lexCount(const std::string &key, const std::string& min, const std::string& max);

    // Returns the size of SortedSet[key]
    int size(const std::string &key);

    // Increments member's score by "by" and returns it
    double incrBy(const std::string &key, const std::string &member, double by);

    // Removes member
    int rem(const std::string &key, const std::string &member);

    // Calculate the union of keys by applying weights and aggregate functions and
    // store in SortedSet[destKey]
    int uni(const std::string& destKey,
            const std::vector<std::string>& keys,
            const std::vector<int>& weights,
            const std::string& aggregate);

    // Calculate the intersection of keys by applying weights and aggregate functions and
    // store in SortedSet[destKey]
    int intersect(const std::string& destKey,
            const std::vector<std::string>& keys,
            const std::vector<int>& weights,
            const std::string& aggregate);

    // Removes element which have ranks in [start, end]
    int remByRank(const std::string &key, int start, int end);

    // Removes element which have scores in [min, max]
    int remByScore(const std::string &key, double min, double max);

    // Removes element which have values in [min, max]
    int remByLex(const std::string &key, const std::string& min, const std::string& max);

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

    std::unordered_map<std::string, SortedSet> mSetMap;
};

};

#endif /* defined(__arag__SortedSetMap__) */
