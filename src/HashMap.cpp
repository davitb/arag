#include "HashMap.h"
#include "AragException.h"

using namespace arag;
using namespace std;

void HashMap::flush()
{
    _hashMap.clear();
}

int HashMap::delKey(const std::string &key)
{
    if (!keyExists(key)) {
        return 0;
    }
    
    return (int)_hashMap.erase(key);
}

bool HashMap::keyExists(const std::string &key)
{
    return _hashMap.count(key) > 0;
}

int HashMap::size()
{
    return (int)_hashMap.size();
}

IMapCommon::ContainerType HashMap::getContainerType()
{
    return IMapCommon::HASH;
}

StringMap& HashMap::get(const std::string& key)
{
    return _hashMap[key];
};

int HashMap::rename(const std::string &key, const std::string &newKey)
{
    _hashMap[newKey].rename(key, newKey);
    _hashMap.erase(key);
    
    return 1;
}

int HashMap::scan(const std::string& key,
                 std::vector<std::pair<std::string, int>>& outArr,
                 const std::string& pattern,
                 int cursor,
                 int timestamp,
                 int upperLimit)
{
    if (!keyExists(key)) {
        throw EInvalidKey();
    }
    
    StringMap& s = _hashMap[key];
    
    return s.scan(outArr, pattern, cursor, timestamp, upperLimit);
}

