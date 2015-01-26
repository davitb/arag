#include "ServerCmds.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "AragServer.h"
#include <sstream>
#include "Database.h"
#include "Config.h"

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
            throw EInvalidArgument();
        }

        string section;
        
        if (cmdNum == Consts::MAX_ARG_NUM) {
            section = mTokens[1].first;
        }
        
        InfoBuilder out;
        
        out.addSection("Server");
        out.addProperty("arag_version", ARAG_VERSION);
        out.addProperty("redis_version", redis_const::REDIS_VERSION);
        out.addProperty("total_commands_processed", to_string(Arag::instance().getTotalCommandsProcessed()));
        
        // FIXME: need to add other info as well
        
        return CommandResultPtr(new CommandResult(out.serialize(),
                                                  RedisProtocol::BULK_STRING));
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr FlushCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    vector<string> out;
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        switch (mCmdType)
        {
            case FLUSHDB:
            {
                Database::instance().flush(ctx.getDatabaseIndex());
                
                return CommandResult::redisOKResult();
            }
                
            case FLUSHALL:
            {
                Database::instance().flush(Database::FLUSH_ALL);
                
                return CommandResult::redisOKResult();
            }
        }
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
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
            throw EInvalidArgument();
        }

        const string& subCommand = mTokens[1].first;
        
        if (subCommand == "SETNAME") {
            if (cmdNum != 3) {
                throw EInvalidArgument();
            }
            
            const string& name = mTokens[2].first;
            
            if (name == "" || name.find(' ') != string::npos) {
                throw EInvalidArgument();
            }
            
            ctx.setClientName(name);
            
            return CommandResult::redisOKResult();
        }
        else
        if (subCommand == "GETNAME") {

            string name = ctx.getClientName();
            
            if (name == "") {
                return CommandResult::redisNULLResult();
            }
            
            return CommandResultPtr(new CommandResult(name, RedisProtocol::BULK_STRING));
        }
        else
        if (subCommand == "LIST") {
            
            vector<SessionContext> sessions = Arag::instance().getSessions();
            
            ClientListBuilder bldr;
            
            for (auto s : sessions) {
                bldr.addRow(s);
            }
            
            return CommandResultPtr(new CommandResult(bldr.serialize(),
                                                      RedisProtocol::BULK_STRING));
        }
        else {
            throw EInvalidCommand(subCommand);
        }
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}


//-------------------------------------------------------------------------

CommandResultPtr ConfigCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }
        
        const string& subCommand = mTokens[1].first;
        
        if (subCommand == "GET") {
            if (cmdNum != 3) {
                throw EInvalidArgument();
            }
            
            const string& param = mTokens[2].first;
            
            if (param == "" || param.find(' ') != string::npos) {
                throw EInvalidArgument();
            }

            // FIXME: Need to support configuration to fulfill values here
            
            vector<pair<string, int>> res;
            
            return CommandResultPtr(new CommandResult(res));
        }
        else
        if (subCommand == "SET") {
            if (cmdNum != 3) {
                throw EInvalidArgument();
            }
            
            const string& param = mTokens[2].first;
            //const string& val = mTokens[3].first;
            
            if (param == "" || param.find(' ') != string::npos) {
                throw EInvalidArgument();
            }
            
            // FIXME: Need to support configuration to fulfill values here
            
            return CommandResult::redisOKResult();
        }

        return CommandResult::redisNULLResult();        
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}

//-------------------------------------------------------------------------

CommandResultPtr SingleArgumentCommand::execute(InMemoryData& data, SessionContext& ctx)
{
    size_t cmdNum = mTokens.size();
    
    try {
        if (cmdNum < Consts::MIN_ARG_NUM || cmdNum > Consts::MAX_ARG_NUM) {
            throw EInvalidArgument();
        }

        switch (mCmdType)
        {
            case DBSIZE:
            {
                InMemoryData& db = Database::instance().get(ctx.getDatabaseIndex());
                
                KeyMap& kmap = db.getKeyMap();
                
                int size = kmap.size();
                
                return CommandResultPtr(new CommandResult(to_string(size),
                                                          RedisProtocol::INTEGER));
            }
                
            case LASTSAVE:
            {
                // FIXME: need properly implement
                return CommandResultPtr(new CommandResult("0",
                                                          RedisProtocol::INTEGER));
            }

            case TIME:
            {
                long secs, msecs;
                Utils::getTimeOfDay(secs, msecs);
                
                vector<pair<string, int>> ret = {
                    make_pair(to_string(secs), RedisProtocol::BULK_STRING),
                    make_pair(to_string(msecs), RedisProtocol::BULK_STRING)
                };
                
                return CommandResultPtr(new CommandResult(ret));
            }
                
        }
        
    }
    catch (std::exception& e) {
        return CommandResult::redisNULLResult();
    }
}
