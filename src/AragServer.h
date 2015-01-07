#ifndef __CacheServer__CacheServer__
#define __CacheServer__CacheServer__

#include <string>
#include <vector>
#include "StringMap.h"
#include "asio.hpp"
#include "Commands.h"
#include "RequestProcessor.h"
#include "Config.h"
#include "SelfTest.h"

namespace arag
{

static const int PORT_NUM = 6379;
static const int MAX_REQUEST_LEN = 512;
    
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

private:
    
    Arag();
    
    ~Arag();
    
    void doAccept();

    static asio::io_service& ioServiceInstance();
    
private:
    RequestProcessor mProcessor;
    asio::ip::tcp::acceptor mAcceptor;
    asio::ip::tcp::socket mSocket;
};
    
}; // arag

#endif /* defined(__CacheServer__CacheServer__) */
