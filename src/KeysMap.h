#ifndef __arag__KeysMap__
#define __arag__KeysMap__

#include <unordered_map>
#include <string>
#include <vector>
#include "IMapCommon.h"
#include "EventPublisher.h"

namespace arag
{
/*
    This class implements a map to store all keys and related inforation.
 */
class KeyMap : public IMapCommon, ISubscriber
{
public:
    
    typedef std::vector<std::pair<std::string, int>> RedisArray;
    
    struct Item
    {
        IMapCommon::TimeBase timeBase;
        IMapCommon::ExpirationType expirationType;
        IMapCommon::ContainerType containerType;
        int expiration;
        int timestamp;

        Item();        
        Item(IMapCommon::ContainerType t);
        Item(IMapCommon::ContainerType t,
             IMapCommon::TimeBase tb,
             IMapCommon::ExpirationType et,
             int exp);
    };
    
    typedef std::unordered_map<std::string, Item> KeyMapType;

    KeyMap();
    
    ~KeyMap();
    
    // Subscribe a map (e.g. SetMap, SortedSetMap, etc) to redirect functions in a batch
    void subscribeMap(IMapCommon& map);
    
    Item& get(const std::string& key);
    
    int add(const std::string& key, Item item);
    
    // Delete only from keyMap. delKey function deletes both from keyMap and
    // other subscribed maps.
    void delOnlyFromKeyMap(const std::string& key);
    
    virtual int size();
    
    // Delete the specified key
    virtual int delKey(const std::string& key);
    
    // Checks if the specified key exists
    virtual bool keyExists(const std::string& key);

    // Get container type - returns NONE
    virtual IMapCommon::ContainerType getContainerType();
    
    // Returns the container type associated with key
    IMapCommon::ContainerType getContainerType(const std::string& key);
    
    // Clears all keys
    virtual void flush();
    
    // Recieve notifications about changed state in DB
    virtual void notify(EventPublisher::Event event, const std::string& key, int db);
    
    // Return human readable container name for the key
    std::string getContainerName(const std::string& key);

    // Return keys that match the given pattern
    RedisArray getKeys(const std::string& pattern);

    // Return a random key
    std::string getRandomKey();
    
private:
    
    KeyMapType _keyMap;
    std::vector<std::reference_wrapper<IMapCommon>> _maps;
    bool _bSubscribed;
    std::unordered_map<int, std::string> _containerMap;
};
    
};

#endif /* defined(__arag__KeysMap__) */
