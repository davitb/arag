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
