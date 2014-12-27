#ifndef __CacheServer__CacheServer__
#define __CacheServer__CacheServer__

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <future>
#include "CSMap.h"
#include "asio.hpp"
#include "Commands.h"
#include "RequestProcessor.h"
#include "SelfTest.h"

namespace arag
{

static const int PORT_NUM = 6379;
static const int MAX_REQUEST_LEN = 512;
    
/*
    Represents a socket session. 
    When an application connects to arag - an object of this class is created.
 */
class Session : public std::enable_shared_from_this<Session>
{
public:
    
    Session(asio::ip::tcp::socket socket, RequestProcessor& rp);

    // Start to listen for commands
    void start();
    
private:
    
    // Read next command
    void doRead();
    
    // Write response to socket
    void doWrite(std::string);
    
private:
    asio::ip::tcp::socket mSocket;
    std::array<char, MAX_REQUEST_LEN> mBuffer;
    std::reference_wrapper<RequestProcessor> mRP;
};

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
