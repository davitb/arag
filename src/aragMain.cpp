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
    if (argc < 2)
    {
        std::cerr << "Usage: [server] | [test]" << std::endl;
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

    std::cerr << "Usage: [server] | [test]" << std::endl;
    
    return 0;
}

