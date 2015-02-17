#ifndef arag_IMapCommon_h
#define arag_IMapCommon_h

namespace arag
{

/*
    An interface that all Maps implement.
 */
class IMapCommon
{
public:

    typedef std::vector<std::pair<std::string, int>> RedisArray;
    
    enum TimeBase
    {
        SEC,
        MSEC
    };
    
    enum ExpirationType
    {
        TIMEOUT,
        TIMESTAMP,
        INFINITE
    };
    
    // Container types supported in Arag.
    // They map directly to Redis types.
    enum ContainerType
    {
        NONE,
        STRING,
        HASH,
        LIST,
        SET,
        SORTEDSET,
        HLL
    };
    
    virtual int size() = 0;

    // Delete the specified key
    virtual int delKey(const std::string& key) = 0;

    // Checks if the specified key exists
    virtual bool keyExists(const std::string& key) = 0;

    // Clears all keys
    virtual void flush() = 0;
    
    // Get container type
    virtual ContainerType getContainerType() = 0;
    
    // Renames the key to newkey
    virtual int rename(const std::string& key, const std::string& newKey) = 0;
    
    // Return sorted container and store in the specified destKey
    virtual void sort(const std::string& key,
                      std::string destKey,
                      bool asc,
                      bool alpha,
                      int offset,
                      int limit,
                      RedisArray& arr) {}
};

};

#endif
