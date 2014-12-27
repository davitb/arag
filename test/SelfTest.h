#ifndef __CacheServer__SelfTest__
#define __CacheServer__SelfTest__

namespace arag
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

}; // arag

#endif /* defined(__CacheServer__SelfTest__) */
