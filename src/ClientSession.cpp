#include <iostream>
#include "ClientSession.h"

using namespace std;
using namespace arag;
using asio::ip::tcp;

ClientSession::ClientSession(tcp::socket socket, RequestProcessor& rp) : mSocket(std::move(socket)), mRP(rp)
{
}

void ClientSession::start()
{
    // Read the incoming request
    doRead();
}

void ClientSession::doRead()
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
                                          mCtx,
                                          responseCallback);
            
            // Enqueue the request to Request Processor
            mRP.get().enqueueRequest(req);
        }
    });
}

void ClientSession::doWrite(string str)
{
    auto self(shared_from_this());
    
    cout << "write result: " << str << endl;
    
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
