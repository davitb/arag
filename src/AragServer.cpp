#include <climits>
#include <ctime>
#include <sstream>
#include <memory>
#include <random>
#include <functional>
#include <iostream>
#include <stdlib.h>
#include "AragServer.h"
#include "Commands.h"

using namespace std;
using namespace arag;
using asio::ip::tcp;

Arag::Arag()
: mAcceptor(Arag::ioServiceInstance(), tcp::endpoint(tcp::v4(), PORT_NUM)), mSocket(Arag::ioServiceInstance())
{
}

Arag::~Arag()
{
}

Arag& Arag::instance()
{
    static Arag cs;
    return cs;
}

asio::io_service& Arag::ioServiceInstance()
{
    static asio::io_service ios;
    return ios;
}

void Arag::startServer()
{
    mProcessor.startThreads();
    
    try {
        // Start listening for connections
        doAccept();
    
        // Run the I/O service. This is a blocking call.
        Arag::ioServiceInstance().run();
    }
    catch (std::exception& e) {
        cout << "ERROR: " << e.what() << endl;
    }
}

void Arag::stopServer()
{
    mProcessor.stopThreads();
}

void Arag::doAccept()
{
    // Accept new connections and start a session when it's established
    mAcceptor.async_accept(mSocket, [this](std::error_code ec) {
        if (!ec) {
            std::make_shared<Session>(std::move(mSocket), mProcessor)->start();
        }

        doAccept();
    });
}

//-----------------------------------------------------------------------------

Session::Session(tcp::socket socket, RequestProcessor& rp) : mSocket(std::move(socket)), mRP(rp)
{
}

void Session::start()
{
    // Read the incoming request
    doRead();
}

void Session::doRead()
{
    auto self(shared_from_this());
    
    // Async read
    mSocket.async_read_some(asio::buffer(mBuffer), [this, self] (std::error_code ec, std::size_t length) {
        if (!ec) {

            string cmdLine(mBuffer.begin(), length);
            
            // This function will be called after response is ready.
            function<void(string)> responseCallback = [this, self](string result) {
                // Write the result string to socket
                doWrite(result);
            };
            
            RequestProcessor::Request req(cmdLine,
                                          RequestProcessor::RequestType::EXTERNAL,
                                          responseCallback);
            
            // Enqueue the request to Request Processor
            mRP.get().enqueueRequest(req);
        }
    });
}

void Session::doWrite(string str)
{
    auto self(shared_from_this());
    
    //cout << "write result: " << str << endl;

    try {
        // This callback function will be called when async write is done
        function<void(std::error_code, std::size_t)> writeFunction =
            [this, self](std::error_code ec, std::size_t len) {
                if (ec) {
                    cout << "response: " << system_error(ec).what() << endl;
                }
                // Start to listen for the next command
                doRead();
        };
        
        asio::async_write(mSocket, asio::buffer(str.c_str(), str.length()), writeFunction);
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
    }
}
