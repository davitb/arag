#include <string>
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono> 
#include "SelfTest.h"
#include "AragServer.h"

using namespace std;
using namespace cache_server;

vector<string> runCommandsAndGetLatestResult(CSMap& map, const vector<string>& cmds)
{
    for (int i = 0; i < cmds.size() - 1; ++i) {
        shared_ptr<Command> pCmd(Command::createCommand(cmds[i]));
        pCmd->execute(map);
    }
    shared_ptr<Command> pCmd(Command::createCommand(cmds[cmds.size() - 1]));
    
    string res = pCmd->execute(map);
    return vector<string>();
}

void printVector(const vector<string>& vec);

void SelfTest::testParser()
{
    // test convertToInt()
    cout << "Running parser tests" << endl;
    string str = "234";
    assert(Command::convertToInt(str) == 234);
    
    str = "-1";
    assert(Command::convertToInt(str) == -1);

    str = "2093402938";
    assert(Command::convertToInt(str) == 2093402938);

    str = "0";
    assert(Command::convertToInt(str) == 0);

    try {
        str = "a";
        Command::convertToInt(str);
        assert(1 == 0);
    }
    catch (invalid_argument& e) {
    }
    
    try {
        str = "1a";
        Command::convertToInt(str);
        assert(1 == 0);
    }
    catch (invalid_argument& e) {
    }

    try {
        str = "a1";
        Command::convertToInt(str);
        assert(1 == 0);
    }
    catch (invalid_argument& e) {
    }
    
    // test parse()
 
    vector<string> tokens;
    
    str = "1 2 3 alskdalksd";
    tokens = {"1", "2", "3", "alskdalksd"};
    assert(Command::parse(str) == tokens);
    
    str = "1";
    tokens = {"1"};
    assert(Command::parse(str) == tokens);

    str = "\"test\"";
    tokens = {"test"};
    assert(Command::parse(str) == tokens);
    
    str = "\"test\" asd";
    tokens = {"test", "asd"};
    assert(Command::parse(str) == tokens);

    str = "asd \"test\"";
    tokens = {"asd", "test"};
    assert(Command::parse(str) == tokens);

    str = "asd1 \"test\" asd2";
    tokens = {"asd1", "test", "asd2"};
    assert(Command::parse(str) == tokens);

    str = "\"test1\" \"test2\"";
    tokens = {"test1", "test2"};
    assert(Command::parse(str) == tokens);
    
    str = "  test1 test2  ";
    tokens = {"test1", "test2"};
    assert(Command::parse(str) == tokens);
    
    try {
        str = "";
        tokens = Command::parse(str);
        assert(0 == 1);
    }
    catch (invalid_argument& e) {
    }
    
    try {
        str = "test \"";
        tokens = Command::parse(str);
        assert(0 == 1);
    }
    catch (invalid_argument& e) {
    }
    
    try {
        str = "\" test";
        tokens = Command::parse(str);
        assert(0 == 1);
    }
    catch (invalid_argument& e) {
    }
    
}

void SelfTest::testBasicCommands()
{
    // test simple set-get
    
    cout << "Running basic commands tests" << endl;
    
    CSMap map;
    vector<string> cmds = {
        "SET k1 test1",
        "GET k1"
    };
    vector<string> res = {"test1"};
    assert(res == runCommandsAndGetLatestResult(map, cmds));

    cmds = {
        "SET k1 test1"
    };
    res = {"5"};
    assert(res == runCommandsAndGetLatestResult(map, cmds));
    
    cmds = {
        "SET k1 test1",
        "SET k2 test2",
        "SET k3 test3",
        "GET k2"
    };
    res = {"test2"};
    assert(res == runCommandsAndGetLatestResult(map, cmds));

    cmds = {
        "SET k1 test1",
        "SET k2 test2",
        "SET k3 test3",
        "MGET k2 k1 k3"
    };
    res = {"test2", "test1", "test3"};
    assert(res == runCommandsAndGetLatestResult(map, cmds));

    cmds = {
        "SET k1 345",
        "INCR k1",
        "GET k1"
    };
    res = {"346"};
    assert(res == runCommandsAndGetLatestResult(map, cmds));

    cmds = {
        "SET k1 345",
        "APPEND k1 test3",
        "GET k1"
    };
    res = {"345test3"};
    assert(res == runCommandsAndGetLatestResult(map, cmds));

    cmds = {
        "SET k1 \"This is a string\"",
        "APPEND k1 test3",
        "GETRANGE k1 3 7"
    };
    res = {"s is "};
    assert(res == runCommandsAndGetLatestResult(map, cmds));
    
    cmds = {
        "SET k1 test1",
        "GETSET k1 test2"
    };
    res = {"test1"};
    assert(res == runCommandsAndGetLatestResult(map, cmds));
}

void SelfTest::testExpiration()
{
    cout << "Running expiration tests" << endl;
    
    CSMap map;
    vector<string> cmds = {
        "SET k1 test1 EX 2",
        "SET k2 test2 EX 4",
        "MGET k1 k2"
    };
    vector<string> res = {"test1", "test2"};
    assert(res == runCommandsAndGetLatestResult(map, cmds));

    this_thread::sleep_for(chrono::seconds(1));

    cmds = {
        "MGET k1 k2"
    };
    res = {"test1", "test2"};
    assert(res == runCommandsAndGetLatestResult(map, cmds));
    
    this_thread::sleep_for(chrono::seconds(2));
    
    cmds = {
        "MGET k1 k2"
    };
    res = {"", "test2"};
    assert(res == runCommandsAndGetLatestResult(map, cmds));
    
    this_thread::sleep_for(chrono::seconds(2));
    
    cmds = {
        "MGET k1 k2"
    };
    res = {"", ""};
    assert(res == runCommandsAndGetLatestResult(map, cmds));
}

void SelfTest::testMultiThreading()
{
    RequestProcessor rp;
    
    // Test with 10 threads
    
    rp.mThreadCount = 10;
    rp.mTriggerCleanupLimit = 2000;
    rp.startThreads();
    
    // Send the same request 1000 times
    for (int i = 0; i < 10000; ++i) {
        string cmdLine = "SET k1 " + to_string(i);
        RequestProcessor::Request req(cmdLine, RequestProcessor::RequestType::EXTERNAL);
        rp.enqueueRequest(req);
    }
    
    rp.stopThreads();
    assert(rp.mData.get("k1") == "9999");
    

    // Test with 10 threads and then stop them in the middle of process
    
    rp.mThreadCount = 10;
    rp.mTriggerCleanupLimit = 2000;
    rp.startThreads();
    
    // Send the same request 1000 times
    for (int i = 0; i < 10000; ++i) {
        string cmdLine = "SET k" + to_string(i) + " " + to_string(i);
        RequestProcessor::Request req(cmdLine, RequestProcessor::RequestType::EXTERNAL);
        rp.enqueueRequest(req);
        
        if (i == 500) {
            rp.stopThreads();
        }
    }

    for (int i = 0; i <= 500; ++i) {
        assert(rp.mData.get("k" + to_string(i)) == to_string(i));
    }
}

void SelfTest::testAll()
{
    testParser();
    
    testBasicCommands();
    
    //testExpiration();
    
    testMultiThreading();
}
