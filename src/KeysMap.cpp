#include "KeysMap.h"
#include "AragException.h"

using namespace std;
using namespace arag;

//----------------------------------------------------------

KeyMap::Item::Item()
{
    ctype = IMapCommon::NONE;
    timestamp = (int)time(0);
    tbase = IMapCommon::SEC;
    timeout = IMapCommon::INFINITE;
}

KeyMap::Item::Item(IMapCommon::ContainerType t)
{
    ctype = t;
    timestamp = (int)time(0);
    tbase = IMapCommon::SEC;
    timeout = IMapCommon::INFINITE;
}

//----------------------------------------------------------

void KeyMap::subscribeMap(IMapCommon& map)
{
    _maps.push_back(map);
}

void KeyMap::flush()
{
    for (int i = 0; i < _maps.size(); ++i) {
        _maps[i].get().flush();
    }
    
    _keyMap.clear();
}

int KeyMap::size()
{
    return (int)_keyMap.size();
}

int KeyMap::delKey(const std::string &key)
{
    for (int i = 0; i < _maps.size(); ++i) {
        _maps[i].get().delKey(key);
    }
    
    return (int)_keyMap.erase(key);
}

void KeyMap::delOnlyFromKeyMap(const std::string &key)
{
    _keyMap.erase(key);
}

IMapCommon::ContainerType KeyMap::getContainerType()
{
    return IMapCommon::NONE;
}

IMapCommon::ContainerType KeyMap::getContainerType(const std::string &key)
{
    if (keyExists(key)) {
        return _keyMap[key].ctype;
    }
    return IMapCommon::NONE;
}

bool KeyMap::keyExists(const std::string& key)
{
    return _keyMap.count(key) > 0;
}

int KeyMap::add(const std::string &key, KeyMap::Item item)
{
    if (keyExists(key)) {
        return 0;
    }
    
    _keyMap[key] = item;
    return 1;
}

KeyMap::Item KeyMap::get(const std::string &key)
{
    if (!keyExists(key)) {
        throw EInvalidKey();
    }
    
    return _keyMap[key];
}

void KeyMap::notify(EventPublisher::Event event, const std::string &key, int db)
{
    try {
        if (event == EventPublisher::Event::del) {
            delOnlyFromKeyMap(key);
            return;
        }
        if (event == EventPublisher::Event::set) {
            add(key, Item(IMapCommon::STRING));
            return;
        }
        if (event == EventPublisher::Event::set_new || event == EventPublisher::Event::sinterstore ||
            event == EventPublisher::Event::sdiffstore || event == EventPublisher::Event::sunionostore) {
            add(key, Item(IMapCommon::SET));
            return;
        }
        if (event == EventPublisher::Event::z_new || event == EventPublisher::Event::zunionstore ||
            event == EventPublisher::Event::zinterstore) {
            add(key, Item(IMapCommon::SORTEDSET));
            return;
        }
        if (event == EventPublisher::Event::list_new) {
            add(key, Item(IMapCommon::LIST));
            return;
        }
        if (event == EventPublisher::Event::hash_new) {
            add(key, Item(IMapCommon::HASH));
            return;
        }
        if (event == EventPublisher::Event::hll_new) {
            add(key, Item(IMapCommon::HLL));
            return;
        }
    }
    catch (...) {}
}
