#include <climits>
#include <algorithm>
#include "ListCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"

using namespace std;
using namespace arag;
using namespace arag::command_const;

//-------------------------------------------------------------------------

string LPushCommand::execute(InMemoryData& data)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string key = mTokens[1].first;
        string value = mTokens[2].first;
        
        ListMap& listMap = data.getListMap();
        int ret = 0;
        
        switch (mCmdType)
        {
            case RPUSH:
            {
                break;
            }
                
            case RPUSHX:
            {
                break;
            }
                
            case LPUSH:
            {
                break;
            }

            case LPUSHX:
            {
                break;
            }
        }
        
        return RedisProtocol::serializeNonArray(to_string(ret), RedisProtocol::DataType::INTEGER);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}
