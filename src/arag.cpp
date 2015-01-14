#include <iostream>
#include "AragServer.h"
#include "SelfTest.h"
#include "RedisProtocol.h"
#include <string>
#include <regex>

using namespace std;
using namespace arag;

using goodliffe::random_access_skip_list;

int main(int argc, char* argv[])
{
    {

//        string s = "*2\r\n$11\r\nUNSUBSCRIBE\r\n$3\r\nfoo\r\n*2\r\n$11\r\nUNSUBSCRIBE\r\n$3\r\nfoo\r\n*2\r\n$11\r\nUNSUBSCRIBE\r\n$3\r\nbar\r\n\x10*2\r\n$11\r\nUNSUBSCRIBE\r\n$3\r\nfoo\r\n";
//        
//        vector<RedisProtocol::RedisArray> arr;
//        RedisProtocol::parse(s, arr);
        
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

