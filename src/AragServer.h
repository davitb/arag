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
    Arag server implementation. All it does is starts to listening to
    appropriate Redis port and accepts connections.
*/
class Arag
{
public:
    
    static Arag& instance();

    void startServer();
    
    void stopServer();
    
    std::vector<SessionContext> getSessions();
    
    ClientSession& getClientSession(int sessionID);
    
    void removeSession(int sessionID);
    
    RequestProcessor& getRequestProcessor();
    
    LuaInterpreter& getLuaInterpreter();

private:
    
    Arag();
    
    ~Arag();
    
    void doAccept();

    static asio::io_service& ioServiceInstance();
    
private:
    RequestProcessor mProcessor;
    LuaInterpreter mLua;
    std::unordered_map<int, std::shared_ptr<ClientSession>> mSessions;
    asio::ip::tcp::acceptor mAcceptor;
    asio::ip::tcp::socket mSocket;
    std::mutex mSessionMapLock;
};
    
}; // arag

#endif /* defined(__CacheServer__CacheServer__) */
