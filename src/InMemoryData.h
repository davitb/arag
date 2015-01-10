#ifndef __arag__InMemoryData__
#define __arag__InMemoryData__

#include <unordered_map>
#include <string>
#include <vector>
#include "StringMap.h"
#include "ListMap.h"
#include "SetMap.h"
#include "SortedSetMap.h"

namespace arag
{

class InMemoryData
{
public:
    
    typedef std::unordered_map<std::string, StringMap> HashMap;
    
    StringMap& getFromHashMap(const std::string& key);
    
    StringMap& getStringMap();
    
    ListMap& getListMap();
    
    SetMap& getSetMap();
    
    SortedSetMap& getSortedSetMap();
    
    int size();
    
    int getCounter() const;
    
    void cleanup();
    
    void flush();
    
    int delKey(const std::string& key);
    
private:
    
    StringMap mStringMap;
    ListMap mListMap;
    HashMap mHashMap;
    SetMap mSetMap;
    SortedSetMap mSortedSetMap;
    int mCounter;
};

    
    
class Database
{
public:

    enum
    {
        FLUSH_ALL = -1
    };
    
    static Database& instance();
    
    Database(int count);
    
    InMemoryData& get(int index);
    
    void flush(int index);
    
private:
    std::vector<InMemoryData> mDatabases;
};


};

#endif /* defined(__arag__InMemoryData__) */
