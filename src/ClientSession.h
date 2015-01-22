#ifndef __arag__ClientSession__
#define __arag__ClientSession__

#include "asio.hpp"
#include "RequestProcessor.h"
#include "SessionContext.h"

namespace arag
{

/*
    ClientSession implementation.
    1) An instance is created when a new connection is accepted by AragServer
    2) Every instance keeps waiting for requests from connected client 
       and writes responses back
    3) Once a request is received it's parsed and enqueued to Request Processor queue
*/
class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:

    enum
    {
        MAX_REQUEST_LEN = 64 * 1024 // 64KB
    };
    
    ClientSession(asio::ip::tcp::socket socket);
    
    // Start to listen for requests
    void start();
    
    SessionContext& getContext();

    // Write response string to the socket
    void writeResponse(const std::string& resp);
    
private:
    
    // Read next request
    void doRead();
    
private:
    asio::ip::tcp::socket mSocket;
    std::array<char, MAX_REQUEST_LEN> mBuffer;
    SessionContext mCtx;
};
    
};

#endif /* defined(__arag__ClientSession__) */
