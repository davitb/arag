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
    
    struct Item
    {
        IMapCommon::TimeBase tbase;
        int timestamp;
        IMapCommon::TimeoutType timeout;
        IMapCommon::ContainerType ctype;

        Item();        
        Item(IMapCommon::ContainerType t);
    };
    
    typedef std::unordered_map<std::string, Item> KeyMapType;

    KeyMap();
    
    ~KeyMap();
    
    void initialize();
    
    void subscribeMap(IMapCommon& map);
    
    Item get(const std::string& key);
    
    int add(const std::string& key, Item item);
    
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
    
private:
    
    KeyMapType _keyMap;
    std::vector<std::reference_wrapper<IMapCommon>> _maps;
    bool _bSubscribed;
};
    
};

#endif /* defined(__arag__KeysMap__) */
