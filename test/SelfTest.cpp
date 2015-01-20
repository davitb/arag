#include <string>
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono> 
#include "SelfTest.h"
#include "AragServer.h"
#include "RedisProtocol.h"
#include "Utils.h"
#include "SessionContext.h"

using namespace std;
using namespace arag;

string runCommandsAndGetLatestResult(InMemoryData& data, const vector<string>& cmds)
{
    SessionContext ctx;
    
    for (int i = 0; i < cmds.size() - 1; ++i) {
        shared_ptr<Command> cmd = Command::getCommand(cmds[i]);
        cmd->execute(data, ctx);
    }
    shared_ptr<Command> cmd = Command::getCommand(cmds[cmds.size() - 1]);
    
    return cmd->execute(data, ctx)->toRedisResponse();
}

vector<string> constructCommands(const vector<vector<string>>& cmds)
{
    vector<pair<string, int>> arr;
    vector<string> finalCmds;

    for (auto cmd: cmds) {
        for (auto arg: cmd) {
            arr.push_back(make_pair(arg, RedisProtocol::DataType::SIMPLE_STRING));
        }
        finalCmds.push_back(RedisProtocol::serializeArray(arr));
        arr = vector<pair<string, int>>();
    }
    
    return finalCmds;
}

void printVector(const vector<string>& vec);

void SelfTest::testParser()
{
    //----------------------------------------------
    // test convertToInt

    cout << "Running parser tests" << endl;
    string str = "234";
    assert(Utils::convertToInt(str) == 234);
    
    str = "-1";
    assert(Utils::convertToInt(str) == -1);

    str = "2093402938";
    assert(Utils::convertToInt(str) == 2093402938);

    str = "0";
    assert(Utils::convertToInt(str) == 0);

    try {
        str = "a";
        Utils::convertToInt(str);
        assert(1 == 0);
    }
    catch (invalid_argument& e) {
    }
    
    try {
        str = "1a";
        Utils::convertToInt(str);
        assert(1 == 0);
    }
    catch (invalid_argument& e) {
    }

    try {
        str = "a1";
        Utils::convertToInt(str);
        assert(1 == 0);
    }
    catch (invalid_argument& e) {
    }
    
    
    //----------------------------------------------
    // test Redis serialization
    
    assert(RedisProtocol::serializeNonArray("OK", RedisProtocol::SIMPLE_STRING) == "+OK\r\n");
    
    assert(RedisProtocol::serializeNonArray("ERROR", RedisProtocol::ERROR) == "-ERROR\r\n");

    assert(RedisProtocol::serializeNonArray("789", RedisProtocol::INTEGER) == ":789\r\n");

    assert(RedisProtocol::serializeNonArray("test", RedisProtocol::BULK_STRING) == "$4\r\ntest\r\n");

    assert(RedisProtocol::serializeNonArray("", RedisProtocol::BULK_STRING) == "$0\r\n\r\n");
    
    vector<pair<string, int>> arr;
    assert(RedisProtocol::serializeArray(arr) == "*0\r\n");
    
    arr.push_back(make_pair("simple", RedisProtocol::DataType::SIMPLE_STRING));
    arr.push_back(make_pair("123", RedisProtocol::DataType::INTEGER));
    arr.push_back(make_pair("bulk", RedisProtocol::DataType::BULK_STRING));
    arr.push_back(make_pair("error", RedisProtocol::DataType::ERROR));
    
    assert(RedisProtocol::serializeArray(arr) == "*4\r\n+simple\r\n:123\r\n$4\r\nbulk\r\n-error\r\n");
    
    //----------------------------------------------
    // test Redis parsing

    vector<pair<string, int>> tokens;
    
    str = "*4\r\n+simple\r\n:123\r\n$4\r\nbulk\r\n-error\r\n";
    tokens = { make_pair("simple", (int)RedisProtocol::DataType::SIMPLE_STRING),
               make_pair("123", (int)RedisProtocol::DataType::INTEGER),
               make_pair("bulk", (int)RedisProtocol::DataType::BULK_STRING),
               make_pair("error", (int)RedisProtocol::DataType::ERROR)
    };
    
    assert(RedisProtocol::parse(str) == tokens);

    try {
        str = "*0\r\n+simple\r\n:123\r\n$4\r\nbulk\r\n-error\r\n";
        RedisProtocol::parse(str);
        assert(1 == 0);
    }
    catch (exception& e) {
    }

    try {
        str = "*1\r\n$3\r\nbulk\r\n";
        RedisProtocol::parse(str);
        assert(1 == 0);
    }
    catch (exception& e) {
    }
}

void SelfTest::testBasicCommands()
{
    // test simple set-get
    
    cout << "Running basic commands tests" << endl;
    
    InMemoryData data;
    vector<string> cmds;
    vector<pair<string, int>> arr;
    vector<vector<string>> args;

    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"SET", "k1", "test1" },
        {"GET", "k1" }
    };
    assert("$5\r\ntest1\r\n" == runCommandsAndGetLatestResult(data, constructCommands(args)));

    
    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"SET", "k1", "test1" }
    };
    assert("+OK\r\n" == runCommandsAndGetLatestResult(data, constructCommands(args)));
    

    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"SET", "k1", "test1" },
        {"SET", "k2", "test2" },
        {"SET", "k3", "test3" },
        {"GET", "k2" }
    };
    assert("$5\r\ntest2\r\n" == runCommandsAndGetLatestResult(data, constructCommands(args)));
    

    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"SET", "k1", "test1" },
        {"SET", "k2", "test2" },
        {"SET", "k3", "test3" },
        {"MGET", "k2", "k1", "k3" }
    };
    assert("*3\r\n$5\r\ntest2\r\n$5\r\ntest1\r\n$5\r\ntest3\r\n" ==
           runCommandsAndGetLatestResult(data, constructCommands(args)));

    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"SET", "k1", "test1" },
        {"MGET", "k1", "none", "none2" }
    };
    assert("*3\r\n$5\r\ntest1\r\n$-1\r\n$-1\r\n" ==
           runCommandsAndGetLatestResult(data, constructCommands(args)));

    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"SET", "k1", "345" },
        {"INCR", "k1" }
    };
    assert(":346\r\n" == runCommandsAndGetLatestResult(data, constructCommands(args)));
    

    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"SET", "k1", "345" },
        {"APPEND", "k1", "test3" },
        {"GET", "k1" }
    };
    assert("$8\r\n345test3\r\n" == runCommandsAndGetLatestResult(data, constructCommands(args)));

    
    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"SET", "k1", "This is a string" },
        {"APPEND", "k1", "test3" },
        {"GETRANGE", "k1", "3", "7" }
    };

    assert("$5\r\ns is \r\n" == runCommandsAndGetLatestResult(data, constructCommands(args)));
    

    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"SET", "k1", "test1" },
        {"GETSET", "k1", "test3" }
    };
    
    assert("$5\r\ntest1\r\n" == runCommandsAndGetLatestResult(data, constructCommands(args)));
}

void SelfTest::testExpiration()
{
    cout << "Running expiration tests" << endl;
    
    InMemoryData data;
    vector<string> cmds;
    vector<pair<string, int>> arr;
    vector<vector<string>> args;
    
    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"SET", "k1", "test1", "EX", "2" },
        {"SET", "k2", "test2", "EX", "4" },
        {"MGET", "k1", "k2" }
    };
    
    assert("*2\r\n$5\r\ntest1\r\n$5\r\ntest2\r\n" ==
           runCommandsAndGetLatestResult(data, constructCommands(args)));
    

    cout << "waiting for 1 secs..." << endl;
    this_thread::sleep_for(chrono::seconds(1));

    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"MGET", "k1", "k2" }
    };
    
    assert("*2\r\n$5\r\ntest1\r\n$5\r\ntest2\r\n" ==
           runCommandsAndGetLatestResult(data, constructCommands(args)));
    

    cout << "waiting for 2 secs..." << endl;
    this_thread::sleep_for(chrono::seconds(2));

    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"MGET", "k1", "k2" }
    };
    
    assert("*2\r\n$-1\r\n$5\r\ntest2\r\n" ==
           runCommandsAndGetLatestResult(data, constructCommands(args)));
    

    cout << "waiting for 2 secs..." << endl;
    this_thread::sleep_for(chrono::seconds(2));
    
    cmds = vector<string>();
    arr = vector<pair<string, int>>();
    args = {
        {"MGET", "k1", "k2" }
    };
    
    assert("*2\r\n$-1\r\n$-1\r\n" ==
           runCommandsAndGetLatestResult(data, constructCommands(args)));
}

void SelfTest::testMultiThreading()
{
    RequestProcessor rp;
    
    // Test with 10 threads
    
    int numRequests = 1000;
    rp.mThreadCount = 1;
    rp.mTriggerCleanupLimit = 2000;
    rp.startThreads();
    
    // Send the same request 1000 times
    for (int i = 0; i < numRequests; ++i) {
        string istr = to_string(i);
        string lenIstr = to_string(istr.length());
        
        string cmdLine = "*3\r\n";
        cmdLine += "$3\r\nSET\r\n";
        cmdLine += "$2\r\nk1\r\n";
        cmdLine += "$" + lenIstr + "\r\n" + istr + "\r\n";
        
        RequestProcessor::Request req(Command::getCommand(cmdLine), SessionContext::FAKE_SESSION);
        rp.enqueueRequest(req);
    }
    
    rp.stopThreads();
    assert(Database::instance().get(0).getStringMap().get("k1") == to_string(numRequests - 1));

    // Test with 10 threads and then stop them in the middle of process
    
    rp.mThreadCount = 1;
    rp.mTriggerCleanupLimit = 2000;
    rp.startThreads();
    
    // Send the same request 1000 times
    for (int i = 0; i < numRequests; ++i) {
        string istr = to_string(i);
        string lenIstr = to_string(istr.length());
        string lenKstr = to_string(istr.length() + 1);
        
        string cmdLine = "*3\r\n$3\r\nSET\r\n";
        cmdLine += "$" + lenKstr + "\r\nk" + istr + "\r\n";
        cmdLine += "$" + lenIstr + "\r\n" + istr + "\r\n";
        
        RequestProcessor::Request req(Command::getCommand(cmdLine), SessionContext::FAKE_SESSION);
        rp.enqueueRequest(req);
        
        if (i == numRequests / 2) {
            rp.stopThreads();
        }
    }

    for (int i = 0; i <= numRequests / 2; ++i) {
        assert(Database::instance().get(0).getStringMap().get("k" + to_string(i)) == to_string(i));
    }
}

void SelfTest::testAll()
{
    testParser();
    
    testBasicCommands();
    
    testExpiration();
    
    testMultiThreading();
}
