#include <iostream>
#include "AragServer.h"
#include "SelfTest.h"
#include "AragClient.h"
#include "linenoise.h"

using namespace std;
using namespace cache_server;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: [server] | [client [hostname{localhost}]] | [test]" << std::endl;
        return 1;
    }
    
    if (string(argv[1]) == "client") {
        
        string host = "localhost";
        if (argc == 3) {
            host = string(argv[2]);
        }
        AragClient client(host);
        client.connectWithCommandLineLoop();
        
        return 1;
    }
    
    if (string(argv[1]) == "server") {
        CacheServer::instance().startServer();
        return 1;
    }

    if (string(argv[1]) == "test") {
        SelfTest test;
        test.testAll();
        return 1;
    }

    std::cerr << "Usage: [server] | [client]" << std::endl;
    
    return 0;
}

