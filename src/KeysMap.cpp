#include "KeysMap.h"
#include "AragException.h"
#include "Database.h"
#include "Utils.h"
#include "RedisProtocol.h"

using namespace std;
using namespace arag;

//----------------------------------------------------------

static bool isExpired(KeyMap::Item& item)
{
    if (item.expiration == 0) {
        return false;
    }
    
    // FIXME: Need to support miliseconds. time() returns in seconds
    
    int currTimestamp = (int)time(0);
    int finalTimestamp = item.timestamp + item.expiration;
    if (item.expirationType == IMapCommon::TIMESTAMP) {
        finalTimestamp = item.expiration;
    }
    if (item.timeBase == IMapCommon::MSEC) {
        currTimestamp *= 1000;
    }
    
    return finalTimestamp <= currTimestamp;
}


//----------------------------------------------------------

KeyMap::Item::Item()
{
    containerType = IMapCommon::NONE;
    timestamp = (int)time(0);
    timeBase = IMapCommon::SEC;
    expirationType = IMapCommon::INFINITE;
    expiration = 0;    
}

KeyMap::Item::Item(IMapCommon::ContainerType t)
{
    containerType = t;
    timestamp = (int)time(0);
    timeBase = IMapCommon::SEC;
    expiration = 0;
    expirationType = IMapCommon::INFINITE;
}

KeyMap::Item::Item(IMapCommon::ContainerType t,
                   IMapCommon::TimeBase tb,
                   IMapCommon::ExpirationType et,
                   int exp)
{
    containerType = t;
    timestamp = (int)time(0);
    timeBase = tb;
    expirationType = et;
    expiration = exp;
}

//----------------------------------------------------------

KeyMap::KeyMap()
{
    _bSubscribed = false;
    
    _containerMap[IMapCommon::HLL] = "hyperloglog";
    _containerMap[IMapCommon::SET] = "set";
    _containerMap[IMapCommon::SORTEDSET] = "zset";
    _containerMap[IMapCommon::LIST] = "list";
    _containerMap[IMapCommon::HASH] = "hash";
    _containerMap[IMapCommon::STRING] = "string";
}

KeyMap::~KeyMap()
{
    Database::instance().getEventPublisher().unsubscribe(this);
}

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
        return _keyMap[key].containerType;
    }
    return IMapCommon::NONE;
}

IMapCommon& KeyMap::getContainer(IMapCommon::ContainerType type)
{
    for (int i = 0; i < _maps.size(); ++i) {
        if (_maps[i].get().getContainerType() == type) {
            return _maps[i];
        }
    }
    
    throw EInvalidArgument();
}

bool KeyMap::keyExists(const std::string& key)
{
    return _keyMap.count(key) > 0;
}

int KeyMap::add(const std::string &key, KeyMap::Item item)
{
    if (keyExists(key)) {
        _keyMap[key] = item;
        return 0;
    }
    
    _keyMap[key] = item;
    return 1;
}

KeyMap::Item& KeyMap::get(const std::string &key)
{
    if (!keyExists(key)) {
        throw EInvalidKey();
    }
    
    return _keyMap[key];
}

std::string KeyMap::getContainerName(const std::string &key)
{
    for (int i = 0; i < _maps.size(); ++i) {
        if (_maps[i].get().keyExists(key)) {
            return _containerMap[_maps[i].get().getContainerType()];
        }
    }
    
    throw EInvalidKey();
}

int KeyMap::getKeys(const std::string &pattern, KeyMap::RedisArray& arr, int cursor, int timestamp, int upperLimit)
{
    if (abs(cursor) >= _keyMap.size()) {
        return 0;
    }
    
    auto elem = _keyMap.begin();
    std::advance(elem, cursor);
    
    while (elem != _keyMap.end() && cursor != upperLimit) {
        if (timestamp > 0 && elem->second.timestamp < timestamp) {
            if (pattern.length() == 0 || (pattern.length() > 0 && Utils::checkPubSubPattern(elem->first, pattern))) {
                arr.push_back(make_pair(elem->first, RedisProtocol::BULK_STRING));
            }
        }
        elem++;
        cursor++;
    }
    
    if (elem == _keyMap.end()) {
        cursor = 0;
    }
    
    return cursor;
}

std::string KeyMap::getRandomKey()
{
    int rand = Utils::genRandom(0, (int)_keyMap.size() - 1);
    auto iter = _keyMap.begin();
    std::advance(iter, rand);
    return iter->first;
}

int KeyMap::rename(const std::string &key, const std::string &newKey)
{
    for (int i = 0; i < _maps.size(); ++i) {
        if (!_maps[i].get().keyExists(key)) {
            continue;
        }

        if (_maps[i].get().rename(key, newKey) == 1) {
            _keyMap[newKey] = _keyMap[key];
            _keyMap.erase(key);
            return 1;
        }
        return 0;
    }
    
    return 0;
}

void KeyMap::delIfExpired(const std::string &key)
{
    if (_keyMap.count(key) == 0) {
        return;
    }
    
    if (isExpired(_keyMap[key])) {
        delKey(key);
    }
}

void KeyMap::notify(EventPublisher::Event event, const std::string &key, int db)
{
    int k = 0;
    k++;
    // When a new key is added/delete in Database -
    // make sure to add/delete them into/from keyMap as well
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
