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
        MAX_REQUEST_LEN = 64 * 1024 // 64KB
    };
    
    ClientSession(asio::ip::tcp::socket socket);
    
    // Start to listen for commands
    void start();
    
    SessionContext& getContext();

    // Write response to socket
    void writeResponse(const std::string& resp);
    
private:
    
    // Read next command
    void doRead();
    
private:
    asio::ip::tcp::socket mSocket;
    std::array<char, MAX_REQUEST_LEN> mBuffer;
    SessionContext mCtx;
};
    
};

#endif /* defined(__arag__ClientSession__) */
