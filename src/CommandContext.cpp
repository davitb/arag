#include "CommandContext.h"

using namespace arag;
using namespace std;

//-----------------------------------------------------------------------------
CommandCtx::CommandCtx()
{
    mContainerType = IMapCommon::NONE;
    type = FIXED;
    numKeysIndex = 0;
}

CommandCtx::CommandCtx(const std::vector<int>& l, IMapCommon::ContainerType ct, CommandCtx::Type t, int num)
{
    mContainerType = ct;
    _keyIndexList = l;
    type = t;
    numKeysIndex = num;
}

bool CommandCtx::hasAnyKey()
{
    return _keyIndexList.size() > 0;
}

bool CommandCtx::commandMatchesContainer(InMemoryData& db,
                                      const CommandResult::ResultArray& tokens)
{
    KeyMap& kmap = db.getKeyMap();
    
    // A function that checks if command's container type correspponds to key's container type
    function<bool(KeyMap&, const string&, int)> fcheck = [](KeyMap& kmap,
                                                            const string& key,
                                                            int containerType) {
        IMapCommon::ContainerType ct = kmap.getContainerType(key);
        if (ct == IMapCommon::NONE) {
            return true;
        }
        
        if (ct != containerType) {
            return false;
        }
        
        return true;
    };
    
    // Apply the function above to all types of arguments
    switch (type)
    {
        case FIXED:
        {
            for (int i = 0; i < _keyIndexList.size(); ++i) {
                if (!fcheck(kmap, tokens[_keyIndexList[i]].first, mContainerType)) {
                    return false;
                }
            }
            break;
        }
            
        case TILL_END:
        {
            for (int i = _keyIndexList[0]; i < tokens.size(); i += 1) {
                if (!fcheck(kmap, tokens[i].first, mContainerType)) {
                    return false;
                }
            }
            break;
        }
            
        case TILL_END_MINUS_ONE:
        {
            for (int i = _keyIndexList[0]; i < tokens.size() - 1; i += 1) {
                if (!fcheck(kmap, tokens[i].first, mContainerType)) {
                    return false;
                }
            }
        }
            
        case EVERY_OTHER:
        {
            for (int i = _keyIndexList[0]; i < tokens.size(); i += 2) {
                if (!fcheck(kmap, tokens[i].first, mContainerType)) {
                    return false;
                }
            }
            break;
        }
            
        case NUM_KEYS:
        {
            if (!fcheck(kmap, tokens[_keyIndexList[0]].first, mContainerType)) {
                return false;
            }
            
            int numKeys = Utils::convertToInt(tokens[numKeysIndex].first);
            
            for (int i = _keyIndexList[0]; i < numKeys; i += 1) {
                if (!fcheck(kmap, tokens[i].first, mContainerType)) {
                    return false;
                }
            }
            break;
        }

        default:
        {
            break;
        }
    }
    
    return true;
}

void CommandCtx::deleteExpiredKeys(InMemoryData& db, const CommandResult::ResultArray& tokens)
{
    KeyMap& kmap = db.getKeyMap();
    
    switch (type)
    {
        case FIXED:
        {
            for (int i = 0; i < _keyIndexList.size(); ++i) {
                kmap.delIfExpired(tokens[_keyIndexList[i]].first);
            }
            break;
        }
            
        case TILL_END:
        {
            for (int i = _keyIndexList[0]; i < tokens.size(); i += 1) {
                kmap.delIfExpired(tokens[i].first);
            }
            break;
        }
            
        case TILL_END_MINUS_ONE:
        {
            for (int i = _keyIndexList[0]; i < tokens.size() - 1; i += 1) {
                kmap.delIfExpired(tokens[i].first);
            }
        }
            
        case EVERY_OTHER:
        {
            for (int i = _keyIndexList[0]; i < tokens.size(); i += 2) {
                kmap.delIfExpired(tokens[i].first);
            }
            break;
        }
            
        case NUM_KEYS:
        {
            kmap.delIfExpired(tokens[_keyIndexList[0]].first);
            
            int numKeys = Utils::convertToInt(tokens[numKeysIndex].first);
            
            for (int i = _keyIndexList[0]; i < numKeys; i += 1) {
                kmap.delIfExpired(tokens[i].first);
            }
            break;
        }
            
        case PATTERN:
        {
            KeyMap::RedisArray arr = kmap.getKeys(tokens[_keyIndexList[0]].first);
            for (int i = 0; i < arr.size(); i++) {
                kmap.delIfExpired(arr[i].first);
            }
            break;
        }
    }
}
