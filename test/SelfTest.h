#ifndef __CacheServer__SelfTest__
#define __CacheServer__SelfTest__

namespace cache_server
{

class SelfTest
{
public:
    
    void testParser();

    void testBasicCommands();
    
    void testExpiration();
    
    void testMultiThreading();
    
    void testAll();
};

}; // cache_server

#endif /* defined(__CacheServer__SelfTest__) */
