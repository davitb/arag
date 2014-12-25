#ifndef __CacheServer__AragClient__
#define __CacheServer__AragClient__

#include <string>
#include <functional>
#include "asio.hpp"
#include "AragClient.h"

namespace cache_server
{

class AragClient
{
public:
    
    AragClient(std::string hostname);
    
    void connectWithCommandLineLoop();
  
private:
    
    void commandLineLoop(asio::ip::tcp::socket& sock, std::function<void(asio::ip::tcp::socket&, std::string cmd)> func);
    
private:
    
    std::string mHostName;
};

};

#endif /* defined(__CacheServer__AragClient__) */
