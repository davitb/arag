#include <iostream>
#include "AragClient.h"
#include "AragServer.h"
#include "linenoise.h"

using namespace std;
using namespace cache_server;
using asio::ip::tcp;

AragClient::AragClient(string hostname)
{
    mHostName = hostname;
}

void static completion(const char *buf, linenoiseCompletions *lc)
{
    string str(buf);
    
    for (string cmd : cache_server::sSupportedCommands) {
        if (cmd.substr(0, str.length()) == str) {
            linenoiseAddCompletion(lc, cmd.c_str());
        }
    }
}

void AragClient::commandLineLoop(tcp::socket& sock, function<void(tcp::socket&, string cmd)> sendToServer)
{
    char *line;
    
    // No multiline support
    linenoiseSetMultiLine(0);
    
    linenoiseHistorySetMaxLen(10);
    
    /* Set the completion callback. This will be called every time the
     * user uses the <tab> key. */
    linenoiseSetCompletionCallback(completion);
    
    /* Load history from file. The history file is just a plain text file
     * where entries are separated by newlines. */
    linenoiseHistoryLoad("history.txt"); /* Load the history at startup */
    
    /* Now this is the main loop of the typical linenoise-based application.
     * The call to linenoise() will block as long as the user types something
     * and presses enter.
     *
     * The typed string is returned as a malloc() allocated string by
     * linenoise, so the user needs to free() it. */
    
    while((line = linenoise("arag> ")) != NULL) {
        
        string lineStr = string(line);
        free(line);
        
        /* Do something with the string. */
        if (lineStr[0] != '\0') {
            linenoiseHistoryAdd(lineStr.c_str()); /* Add to the history. */
            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
            
            if (lineStr== "exit" || lineStr == "q" || lineStr == "quit") {
                break;
            }
            
            // Send the command to server
            sendToServer(sock, lineStr);
            
        } else if (lineStr[0] == '/') {
            printf("Unreconized command: %s\n", line);
        }
    }
}

void AragClient::connectWithCommandLineLoop()
{
    try
    {
        cout << "connecting to " + mHostName << endl;
        asio::io_service io_service;
        
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(mHostName, to_string(cache_server::PORT_NUM));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        tcp::socket socket(io_service);
        asio::connect(socket, endpoint_iterator);

        cout << "---connected---" << endl;
        
        commandLineLoop(socket, [] (tcp::socket& socket, string cmd) {
            asio::error_code error;
            
            asio::write(socket, asio::buffer(cmd), error);
            if (error)
                throw asio::system_error(error); // Some other error.
            
            std::array<char, cache_server::MAX_REQUEST_LEN> buf;
            
            size_t len = socket.read_some(asio::buffer(buf), error);
            if (error == asio::error::eof)
                return; // Connection closed cleanly by peer.
            else if (error)
                throw asio::system_error(error); // Some other error.

            cout << endl;
            std::cout.write(buf.data(), len);
            cout << endl << endl;
        });
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
