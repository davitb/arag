#ifndef __CacheServer__CacheServer__
#define __CacheServer__CacheServer__

#include <string>
#include <vector>
#include "asio.hpp"
#include "RequestProcessor.h"
#include "Config.h"
#include "ClientSession.h"
#include "SessionContext.h"
#include "LuaInterpreter.h"
#include "SelfTest.h"

namespace arag
{

static const int PORT_NUM = 6379;
    
/*
    Arag server implementation. 
    This is a singleton class. It has several responsibilities:
    1) Starts listening to Redis port and accepts connections
    2) Keeps a map of client sessions
    3) Initializes and keeps the only instance of LUA interpreter
    4) Initializes and keeps the only instance of Request Processor
    5) Keeps a static instance of asio::IOService
*/
class Arag
{
public:
    
    static Arag& instance();

    void startServer();
    
    void stopServer();
    
    // Return a copy of client session contexts
    std::vector<SessionContext> getSessions();

    ClientSession& getClientSession(int sessionID);
    
    void removeSession(int sessionID);
    
    RequestProcessor& getRequestProcessor();
    
    LuaInterpreter& getLuaInterpreter();

    void incrByTotalCommandsProcessed(unsigned int by)
    {
        _totalCommandsProcessed += by;
    }
    
    unsigned int getTotalCommandsProcessed() const
    {
        return _totalCommandsProcessed;
    }
    
private:
    
    Arag();
    
    ~Arag();
    
    void doAccept();

    // Returns reference to static IO service
    asio::io_service& ioService();
    
private:
    RequestProcessor mProcessor;
    LuaInterpreter mLua;
    std::unordered_map<int, std::shared_ptr<ClientSession>> mSessions;
    std::mutex mSessionMapLock;
    asio::ip::tcp::acceptor mAcceptor;
    asio::ip::tcp::socket mSocket;
    static asio::io_service sIOS;
    unsigned int _totalCommandsProcessed;
};
    
}; // arag

#endif /* defined(__CacheServer__CacheServer__) */
