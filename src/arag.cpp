#include <iostream>
#include "AragServer.h"
#include "SelfTest.h"
#include "RedisProtocol.h"
#include <string>
#include <set>

using namespace std;
using namespace arag;

struct Item
{
    int score;
    string val;
    
    Item(string val, int score)
    {
        this->score = score;
        this->val = val;
    }
};

int main(int argc, char* argv[])
{
    {
//        function<bool(const Item&, const Item&)> criterion = [](const Item& i1, const Item& i2) {
//            return i2.score >= i1.score && i1.val != i2.val;
//        };
//        set<Item, function<bool(const Item&, const Item&)>> s(criterion);
//        
//        s.insert(Item("one", 1));
//        s.insert(Item("uno", 1));
//        s.insert(Item("uno", 1));
//        s.insert(Item("three", 3));
//        s.insert(Item("two", 2));
//        
//        
//        
//        for_each(s.begin(), s.end(), [](Item i) {
//            cout << i.val << " " << i.score << endl;
//        });
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

