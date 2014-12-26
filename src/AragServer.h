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

namespace cache_server
{

static const int PORT_NUM = 6379;
static const int MAX_REQUEST_LEN = 512;
    
class Session : public std::enable_shared_from_this<Session>
{
public:
    
    Session(asio::ip::tcp::socket socket, RequestProcessor& rp);
    
    void start();
    
private:
    
    void doRead();
    
    void doWrite(std::string);
    
private:
    asio::ip::tcp::socket mSocket;
    std::array<char, MAX_REQUEST_LEN> mBuffer;
    std::reference_wrapper<RequestProcessor> mRP;
};

class CacheServer
{
public:
    
    static CacheServer& instance();

    void runCommand(std::string cmdLine, std::function<void(std::string)> cb);
    
    void startServer();
    
    void stopServer();

private:
    
    CacheServer();
    
    ~CacheServer();
    
    void doAccept();

    static asio::io_service& ioServiceInstance();
    
private:
    RequestProcessor mProcessor;
    asio::ip::tcp::acceptor mAcceptor;
    asio::ip::tcp::socket mSocket;
};
    
}; // cache_server

#endif /* defined(__CacheServer__CacheServer__) */
