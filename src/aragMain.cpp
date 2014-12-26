#include <iostream>
#include "AragServer.h"
#include "SelfTest.h"
#include "AragClient.h"
#include "linenoise.h"
#include "RedisProtocol.h"

using namespace std;
using namespace cache_server;

void printVec(const vector<string>& toks)
{
    cout << toks.size() << endl;
    for (auto tok : toks) {
        cout << tok << " ";
    }
    cout << endl;
}

int main(int argc, char* argv[])
{
    {
//        string str = "*2\r\n$4\r\nLLEN\r\n$6\r\nmylist\r\n";
//        str = "*3\r\n$3\r\nSET\r\n+k1\r\n+123\r\n";
//        printVec(RedisProtocol::parse(str));
//        return 1;
    }
    
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

