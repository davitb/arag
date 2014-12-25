#include <iostream>
#include "AragServer.h"
#include "SelfTest.h"
#include "AragClient.h"

using namespace std;
using namespace cache_server;

void printVector(const vector<string>& vec)
{
    for_each (vec.begin(), vec.end(), [](string s) {
        cout << s << endl;
    });
}

void selfTest()
{
    SelfTest test;
    test.testAll();
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: [server] | [client]" << std::endl;
        return 1;
    }
    
    if (string(argv[1]) == "client") {
        
        AragClient client("localhost");
        client.connectWithCommandLineLoop();
        
        return 1;
    }
    
    if (string(argv[1]) == "server") {
        CacheServer::instance().startServer();
        return 1;
    }
    
    return 0;
}

