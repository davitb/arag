#include <iostream>
#include "AragServer.h"
#include "SelfTest.h"
#include "RedisProtocol.h"
#include <string>
#include "LuaInterpreter.h"
#include "AragException.h"

using namespace std;
using namespace arag;

int main(int argc, char* argv[])
{
    {
//        try {
//            throw EInvalidArgument();
//        }
//        catch (AragException& e) {
//            cout << e.what();
//        }
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

