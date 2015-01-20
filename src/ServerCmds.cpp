#include "ServerCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "AragServer.h"
#include <sstream>

using namespace std;
using namespace arag;

//-------------------------------------------------------------------------

class InfoBuilder
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

CommandResultPtr InfoCommand::execute(InMemoryData& data, SessionContext& ctx)
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
        
        InfoBuilder out;
        
        out.addSection("Server");
        out.addProperty("redis_version", redis_const::REDIS_VERSION);
        
        return CommandResultPtr(new CommandResult(out.serialize(), RedisProtocol::DataType::BULK_STRING));
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr FlushCommand::execute(InMemoryData& data, SessionContext& ctx)
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
                
                return CommandResultPtr(new CommandResult("OK", RedisProtocol::DataType::SIMPLE_STRING));
            }
                
            case FLUSHALL:
            {
                Database::instance().flush(Database::FLUSH_ALL);
                
                return CommandResultPtr(new CommandResult("OK", RedisProtocol::DataType::SIMPLE_STRING));
            }
        }
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

class ClientListBuilder
{
public:
    void addRow(SessionContext& ctx)
    {
        ss << "addr=" + ctx.getIPAndPort() << " ";
        ss << "name=" + ctx.getClientName() << " ";
        ss << "db=" + to_string(ctx.getDatabaseIndex());
        ss << "\r\n";
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

CommandResultPtr ClientCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        string subCommand = mTokens[1].first;
        
        if (subCommand == "SETNAME") {
            if (cmdNum != 3) {
                throw invalid_argument("Invalid number of args");
            }
            
            string name = mTokens[2].first;
            
            if (name == "" || name.find(' ') != string::npos) {
                throw invalid_argument("Invalid client name");
            }
            
            ctx.setClientName(name);
            
            return CommandResultPtr(new CommandResult("OK", RedisProtocol::DataType::SIMPLE_STRING));
        }
        else
        if (subCommand == "GETNAME") {

            string name = mTokens[2].first;
            
            if (name == "") {
                return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
            }
            
            return CommandResultPtr(new CommandResult(name, RedisProtocol::DataType::BULK_STRING));
        }
        else
        if (subCommand == "LIST") {
            
            vector<SessionContext> sessions = Arag::instance().getSessions();
            
            ClientListBuilder bldr;
            
            for (auto s : sessions) {
                bldr.addRow(s);
            }
            
            return CommandResultPtr(new CommandResult(bldr.serialize(), RedisProtocol::DataType::BULK_STRING));
        }
        else {
            throw invalid_argument("SubCommand not supported");
        }
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}


//-------------------------------------------------------------------------

CommandResultPtr ConfigCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }
        
        string subCommand = mTokens[1].first;
        
        if (subCommand == "GET") {
            if (cmdNum != 3) {
                throw invalid_argument("Invalid number of args");
            }
            
            string param = mTokens[2].first;
            
            if (param == "" || param.find(' ') != string::npos) {
                throw invalid_argument("Invalid config param");
            }

            // FIXME: Need to support configuration to fulfill values here
            
            vector<pair<string, int>> res;
            
            return CommandResultPtr(new CommandResult(res));
        }
        else
        if (subCommand == "SET") {
            if (cmdNum != 3) {
                throw invalid_argument("Invalid number of args");
            }
            
            string param = mTokens[2].first;
            string val = mTokens[3].first;
            
            if (param == "" || param.find(' ') != string::npos) {
                throw invalid_argument("Invalid config param");
            }
            
            // FIXME: Need to support configuration to fulfill values here
            
            return CommandResultPtr(new CommandResult("OK", RedisProtocol::DataType::SIMPLE_STRING));
        }

        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));        
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SingleArgumentCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw invalid_argument("Invalid args");
        }

        switch (mCmdType)
        {
            case DBSIZE:
            {
                InMemoryData& db = Database::instance().get(ctx.getDatabaseIndex());
                
                int size = db.size();
                
                return CommandResultPtr(new CommandResult(to_string(size), RedisProtocol::DataType::INTEGER));
            }
                
            case LASTSAVE:
            {
                return CommandResultPtr(new CommandResult("0", RedisProtocol::DataType::INTEGER));
            }

            case TIME:
            {
                long secs, msecs;
                Utils::getTimeOfDay(secs, msecs);
                
                vector<pair<string, int>> ret = {
                    make_pair(to_string(secs), RedisProtocol::DataType::BULK_STRING),
                    make_pair(to_string(msecs), RedisProtocol::DataType::BULK_STRING)
                };
                
                return CommandResultPtr(new CommandResult(ret));
            }
                
        }
        
    }
    catch (std::exception& e) {
        return CommandResultPtr(new CommandResult(redis_const::NULL_BULK_STRING, RedisProtocol::DataType::NILL));
    }
}
