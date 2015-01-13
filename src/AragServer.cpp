#include <climits>
#include <ctime>
#include <sstream>
#include <memory>
#include <random>
#include <functional>
#include <iostream>
#include <stdlib.h>
#include "AragServer.h"
#include "Commands.h"
#include "ClientSession.h"

using namespace std;
using namespace arag;
using asio::ip::tcp;

Arag::Arag()
: mAcceptor(Arag::ioServiceInstance(), tcp::endpoint(tcp::v4(), PORT_NUM)), mSocket(Arag::ioServiceInstance())
{
}

Arag::~Arag()
{
}

Arag& Arag::instance()
{
    static Arag cs;
    return cs;
}

asio::io_service& Arag::ioServiceInstance()
{
    static asio::io_service ios;
    return ios;
}

void Arag::startServer()
{
    mProcessor.startThreads();
    
    try {
        // Start listening for connections
        doAccept();
    
        // Run the I/O service. This is a blocking call.
        Arag::ioServiceInstance().run();
    }
    catch (std::exception& e) {
        cout << "ERROR: " << e.what() << endl;
    }
}

void Arag::stopServer()
{
    mProcessor.stopThreads();
}

void Arag::doAccept()
{
    // Accept new connections and start a session when it's established
    mAcceptor.async_accept(mSocket, [this](std::error_code ec) {
        if (!ec) {
            shared_ptr<ClientSession> session = std::make_shared<ClientSession>(std::move(mSocket), mProcessor);
            mSessions.push_back(session);
            session->start();
//            std::make_shared<ClientSession>(std::move(mSocket), mProcessor)->start();
        }

        doAccept();
    });
}

vector<SessionContext> Arag::getSessions()
{
    vector<SessionContext> sessions;
    
    for (int i = 0; i < mSessions.size(); ++i) {
        sessions.push_back(mSessions[i]->getContext());
    }
    
    return sessions;
}
