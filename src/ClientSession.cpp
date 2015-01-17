#include <iostream>
#include "ClientSession.h"
#include "RedisProtocol.h"
#include "AragServer.h"
#include <regex>

using namespace std;
using namespace arag;
using asio::ip::tcp;

ClientSession::ClientSession(tcp::socket socket, RequestProcessor& rp)
            : mSocket(std::move(socket)), mRP(rp)
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

        if (ec) {
            Arag::instance().removeSession(mCtx.getSessionID());
            return;
        }
        
        try {
        
            string cmdLine = string(mBuffer.begin(), length);
            
//            cout << "SessionID: " << mCtx.getSessionID() << " ";
            cout << std::regex_replace(cmdLine, std::regex("(\r\n)"),"\\r\\n") << endl << endl;

            // If the request is more than default MAX_REQUEST_LEN - read the remaining here
            size_t available = 0;
//            while ((available = mSocket.available()) > 0) {
//                cout << "------------------------------------------" << endl;
//                std::vector<char> data(available);
//                asio::read(mSocket, asio::buffer(data));
//                cmdLine += string(mBuffer.begin(), length) + string(&data[0]);
//            }

            mCtx.setConnectionDetails(mSocket.remote_endpoint().address().to_string(),
                                      mSocket.remote_endpoint().port());
            
            vector<shared_ptr<Command>> cmds;
            Command::getCommand(cmdLine, cmds);
            for (int i = 0; i < cmds.size(); ++i) {
                RequestProcessor::Request req(cmds[i], mCtx.getSessionID());
                
                // Enqueue the request to Request Processor
                mRP.get().enqueueRequest(req);
            }
        }
        catch (std::exception& e) {
            cout << e.what() << endl;
            // Send ERROR back to the client
            writeResponse(redis_const::ERR_GENERIC);
        }
        
        // Read the next request
        doRead();        
    });
}

void ClientSession::writeResponse(const std::string &str)
{
    auto self(shared_from_this());
    
    if (str.length() == 0) {
        return;
    }
    
    cout << std::regex_replace(str, std::regex("(\r\n)"),"\\r\\n") << endl << endl;
    
    try {
        // This callback function will be called when async write is done
        function<void(std::error_code, std::size_t)> writeFunction =
        [this, self](std::error_code ec, std::size_t len) {
            if (ec) {
                cout << "response: " << system_error(ec).what() << endl;
            }
        };
        
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
