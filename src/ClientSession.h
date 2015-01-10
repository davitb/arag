#ifndef __arag__ClientSession__
#define __arag__ClientSession__

#include "asio.hpp"
#include "RequestProcessor.h"
#include "SessionContext.h"

namespace arag
{

/*
 Represents a socket session.
 When an application connects to arag - an object of this class is created.
 */
class ClientSession : public std::enable_shared_from_this<ClientSession>
{
public:

    enum
    {
        MAX_REQUEST_LEN = 512 // * 64 * 1024 // 640KB
    };
    
    ClientSession(asio::ip::tcp::socket socket, RequestProcessor& rp);
    
    // Start to listen for commands
    void start();
    
    SessionContext getContext();
    
private:
    
    // Read next command
    void doRead();
    
    // Write response to socket
    void doWrite(std::string);
    
private:
    asio::ip::tcp::socket mSocket;
    std::array<char, MAX_REQUEST_LEN> mBuffer;
    std::reference_wrapper<RequestProcessor> mRP;
    SessionContext mCtx;
};
    
};

#endif /* defined(__arag__ClientSession__) */
