#include <iostream>
#include "AragServer.h"
#include "SelfTest.h"
#include "RedisProtocol.h"
#include <string>
#include <set>
#define SKIP_LIST_IMPL_DIAGNOSTICS
#include "random_access_skip_list.h"

using namespace std;
using namespace arag;

struct TestItem
{
    int score;
    string val;
    bool bSearching;

    TestItem(int s)
    {
        this->score = s;
        bSearching = true;
    }
    
    TestItem(string val, int score, bool search = false)
    {
        this->score = score;
        this->val = val;
        bSearching = search;
    }
    
    /*
     {2, "dav"} // NO
     {1, "dav"} // NO
     
     {1, "dav"}
     {1, "dav2"}     
     {3, "dav3"}
     */
    struct Less {
        bool operator()(const TestItem &lhs, const TestItem &rhs) const
        {
            if (lhs.score < rhs.score) {
                return true;
            }
            if (lhs.score == rhs.score && lhs.val.compare(rhs.val) < 0) {
                return true;
            }
            if (lhs.score == rhs.score && lhs.val == rhs.val) {
                return false;
            }
            return false;
        }
    };
};

using goodliffe::random_access_skip_list;

int main(int argc, char* argv[])
{
    {
//        random_access_skip_list<TestItem, TestItem::Less> sl;
//        
//        sl.insert(TestItem("dav21", 2));
//        sl.insert(TestItem("dav3", 3));
//        sl.insert(TestItem("dav22", 1));
//        sl.insert(TestItem("dav1", 1));
//        
////        cout << sl[2].val << endl;
////        
////        cout << (*sl.find(2)).val << endl;
//        
//        sl.erase(TestItem("dav22", 1, true));
//        
//        for (auto item : sl) {
//            cout << item.val << " " << item.score << endl;
//        }
//        
//        cout << endl;
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

