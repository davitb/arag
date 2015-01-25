#include <iostream>
#include "ClientSession.h"
#include "RedisProtocol.h"
#include "AragServer.h"
#include <regex>

using namespace std;
using namespace arag;
using asio::ip::tcp;

ClientSession::ClientSession(tcp::socket socket)
            : mSocket(std::move(socket))
{
}

void ClientSession::start()
{
    // Remember connection information in the context
    mCtx.setConnectionDetails(mSocket.remote_endpoint().address().to_string(),
                              mSocket.remote_endpoint().port());
    
    // Read the incoming request
    doRead();
}

void ClientSession::doRead()
{
    auto self(shared_from_this());
    
    // Async read
    mSocket.async_read_some(asio::buffer(mBuffer), [this, self] (std::error_code ec, std::size_t length) {

        if (ec) {
            // If the client is diconnected - remove it from global list
            Arag::instance().removeSession(mCtx.getSessionID());
            return;
        }
        
        try {
        
            string cmdLine = string(mBuffer.begin(), length);
            
            if (cmdLine.length() < 1000) {
//                cout << "SessionID: " << mCtx.getSessionID() << " ";
//                cout << std::regex_replace(cmdLine, std::regex("(\r\n)"),"\\r\\n") << endl << endl;
            }
            
            // If the request is more than default MAX_REQUEST_LEN - read the remaining here
//            size_t available = 0;
//            while ((available = mSocket.available()) > 0) {
//                cout << "------------------------------------------" << endl;
//                std::vector<char> data(available);
//                asio::read(mSocket, asio::buffer(data));
//                cmdLine += string(mBuffer.begin(), length) + string(&data[0]);
//            }
            
            // Parse the incoming request into command(s) and then enqueue to RP's queue
            vector<shared_ptr<Command>> cmds;
            Command::getCommand(cmdLine, cmds);
            for (int i = 0; i < cmds.size(); ++i) {
                // Create a request
                RequestProcessor::Request req(cmds[i], mCtx.getSessionID());
                // Enqueue the request to Request Processor
                Arag::instance().getRequestProcessor().enqueueRequest(req);
            }
        }
        catch (std::exception& e) {
            cout << e.what() << endl;
            // Send ERROR back to the client
            writeResponse(RedisProtocol::serializeNonArray(e.what(), RedisProtocol::ERROR));
        }
        
        // Read the next request
        doRead();        
    });
}

void ClientSession::writeResponse(const std::string &str)
{
    auto self(shared_from_this());
    
    // If the response is empty - don't send it back
    if (str.length() == 0) {
        return;
    }
    
//    cout << std::regex_replace(str, std::regex("(\r\n)"),"\\r\\n") << endl << endl;
    
    try {
        // This callback function will be called when async write is done
        function<void(std::error_code, std::size_t)> writeFunction =
        [this, self](std::error_code ec, std::size_t len) {
            if (ec) {
                cout << "response: " << system_error(ec).what() << endl;
            }
        };
        
        // Send the response to client
        asio::async_write(mSocket, asio::buffer(str.c_str(), str.length()), writeFunction);
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
    }
}

SessionContext& ClientSession::getContext()
{
    return mCtx;
}
