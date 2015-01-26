#ifndef __arag__CommandContext__
#define __arag__CommandContext__

#include <string>
#include <vector>
#include "IMapCommon.h"
#include "CommandResult.h"
#include "InMemoryData.h"

namespace arag
{

/*
 Keeps a context information about a command
 */
class CommandCtx
{
public:
    
    enum Type
    {
        // Indicates keys are fixed
        FIXED,
        // Indicates every other argument is a key
        EVERY_OTHER,
        // Indicates all arguments till the end are keys
        TILL_END,
        // Indicates all arguments till the end -1 are keys
        TILL_END_MINUS_ONE,
        // Indicates number of arguments that are keys
        NUM_KEYS,
        // Indicates keys macthing the given pattern
        PATTERN
    };
    
    CommandCtx();
    
    CommandCtx(const std::vector<int>& l,
            IMapCommon::ContainerType ct,
            CommandCtx::Type t = Type::FIXED,
            int num = -1);
    
    // Checks if the keys in command arguments match with the container this command
    // will be operating on
    bool commandMatchesContainer(InMemoryData& db, const CommandResult::ResultArray& tokens);
    
    // Checks if the command has any key index registered
    bool hasAnyKey();
    
    // Delete expired keys
    void deleteExpiredKeys(InMemoryData& db, const CommandResult::ResultArray& tokens);
    
private:
    std::vector<int> _keyIndexList;
    IMapCommon::ContainerType mContainerType;
    Type type;
    int numKeysIndex;
};
    
};

#endif /* defined(__arag__CommandContext__) */
