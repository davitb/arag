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
    
//    // Dumps the value for the given key
//    virtual std::string dump(const std::string& key) = 0;
//    
//    // Resotores the value for the given key
//    virtual void restore(const std::string& key, const std::string& dumpedVal) = 0;
    
    // Set a timeout on the key
//    virtual int setTimeout(const std::string& key,
//                           int timeout,
//                           ExpirationType ttype,
//                           TimeBase tbase);

    // Returns time to live
//    virtual int ttl(const std::string& key, TimeBase tbase) = 0;

};

};

#endif
