#include <iostream>
#include "AragServer.h"
#include "SelfTest.h"
#include "RedisProtocol.h"
#include <string>
#include "HLLMap.h"

using namespace std;
using namespace arag;

using goodliffe::random_access_skip_list;

int main(int argc, char* argv[])
{
    {
//        HLLMap hll;
//        hll.add("hll", "foo");
//        hll.add("hll", "bar");
//        cout << hll.add("hll", "zap");
//
//        cout << hll.count("hll") << endl;
//        
//        cout << hll.add("hll", "foo");
//        cout << hll.add("hll", "bar");
//        cout << hll.add("hll", "zap");
//        
//        cout << hll.count("hll") << endl;
//        
//        return 1;
    }
    
    try {
        if (argc < 2)
        {
            std::cerr << "Usage: [server] | [test]" << std::endl;
            return 1;
        }
        
        if (string(argv[1]) == "server") {
            Arag::instance().startServer();
            return 1;
        }

        if (string(argv[1]) == "test") {
            SelfTest test;
            test.testAll();
            return 1;
        }

        std::cerr << "Usage: [server] | [test]" << std::endl;
    }
    catch (std::exception& e) {
        cout << "ERROR: " << e.what() << endl;
    }
    
    return 0;
}

