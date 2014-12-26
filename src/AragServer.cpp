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
using namespace cache_server;
using asio::ip::tcp;

CacheServer::CacheServer()
: mAcceptor(CacheServer::ioServiceInstance(), tcp::endpoint(tcp::v4(), PORT_NUM)), mSocket(CacheServer::ioServiceInstance())
{
}

CacheServer::~CacheServer()
{
}

CacheServer& CacheServer::instance()
{
    static CacheServer cs;
    return cs;
}

asio::io_service& CacheServer::ioServiceInstance()
{
    static asio::io_service ios;
    return ios;
}

void CacheServer::runCommand(std::string cmdLine, std::function<void(std::string)> cb)
{
    RequestProcessor::Request req(cmdLine, RequestProcessor::RequestType::EXTERNAL, cb);
    mProcessor.enqueueRequest(req);
}

void CacheServer::startServer()
{
    mProcessor.startThreads();
    
    try {
        doAccept();
    
        CacheServer::ioServiceInstance().run();
    }
    catch (std::exception& e) {
        cout << "ERROR: " << e.what() << endl;
    }
}

void CacheServer::stopServer()
{
    mProcessor.stopThreads();
}

void CacheServer::doAccept()
{
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
    
    // async read
    mSocket.async_read_some(asio::buffer(mBuffer), [this, self] (std::error_code ec, std::size_t length) {
        if (!ec) {
            // enqueue
            string cmdLine(mBuffer.begin(), length);
            
            RequestProcessor::Request req(cmdLine, RequestProcessor::RequestType::EXTERNAL, [this, self](string result) {
                
                doWrite(result);
            });
            
            mRP.get().enqueueRequest(req);
        }
    });
}

void Session::doWrite(string str)
{
    auto self(shared_from_this());
    
    cout << "write result: " << str << endl;

    try {
        asio::async_write(mSocket, asio::buffer(str.c_str(), str.length()), [this, self](std::error_code ec, std::size_t len) {
            if (ec) {
                cout << "response: " << system_error(ec).what() << endl;
            }
            doRead();
        });
    }
    catch (std::exception& e) {
        cout << e.what() << endl;
    }
}
