#include "ServerCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include <sstream>

using namespace std;
using namespace arag;

//-------------------------------------------------------------------------

class InfoOutput
{
public:
    void addSection(const std::string& section)
    {
        ss << "# " + section + "\r\n";
    }
    
    void addProperty(const std::string& property, const std::string& val)
    {
        ss << property << ":" << val << "\r\n";
    }
    
    std::string serialize()
    {
        return ss.str();
    }
    
    void clear()
    {
        ss.clear();
    }
    
private:
    stringstream ss;
};

string InfoCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        string section;
        
        if (cmdNum == Consts::MAX_ARG_NUM) {
            section = mTokens[1].first;
        }
        
        InfoOutput out;
        
        out.addSection("Server");
        out.addProperty("redis_version", "2.6.14");
        
        return RedisProtocol::serializeNonArray(out.serialize(), RedisProtocol::DataType::BULK_STRING);
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}

//-------------------------------------------------------------------------

string FlushCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        switch (mCmdType)
        {
            case FLUSHDB:
            {
                Database::instance().flush(ctx.getDatabaseIndex());
                
                return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
            }
                
            case FLUSHALL:
            {
                Database::instance().flush(Database::FLUSH_ALL);
                
                return RedisProtocol::serializeNonArray("OK", RedisProtocol::DataType::SIMPLE_STRING);
            }
        }
    }
    catch (std::exception& e) {
        return redis_const::NULL_BULK_STRING;
    }
}
