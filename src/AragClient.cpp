#include <iostream>
#include "AragClient.h"
#include "AragServer.h"

using namespace std;
using namespace cache_server;
using asio::ip::tcp;

AragClient::AragClient(string hostname)
{
    mHostName = hostname;
}

void AragClient::commandLineLoop(tcp::socket& sock, function<void(tcp::socket&, string cmd)> func)
{
    string in;
    
    while (true) {
        
        getline(cin, in);
        if (in == "exit") {
            break;
        }
        
        func(sock, in);
    }
}

void AragClient::connectWithCommandLineLoop()
{
    try
    {
        asio::io_service io_service;
        
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(mHostName, to_string(cache_server::PORT_NUM));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);
        asio::connect(socket, endpoint_iterator);
        
        commandLineLoop(socket, [] (tcp::socket& socket, string cmd) {
            asio::error_code error;
            
            asio::write(socket, asio::buffer(cmd), error);
            if (error)
                throw asio::system_error(error); // Some other error.
            
            std::array<char, cache_server::MAX_REQUEST_LEN> buf;
            
            size_t len = socket.read_some(asio::buffer(buf), error);
            if (error == asio::error::eof)
                return; // Connection closed cleanly by peer.
            else if (error)
                throw asio::system_error(error); // Some other error.
            
            cout << "Server response: ";
            std::cout.write(buf.data(), len);
            cout << endl;
        });
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
