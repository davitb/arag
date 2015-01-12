#ifndef arag_IMapCommon_h
#define arag_IMapCommon_h

namespace arag
{

class IMapCommon
{
public:
    virtual int size() = 0;
    
    virtual int delKey(const std::string& key) = 0;
    
    virtual bool keyExists(const std::string& key) = 0;
    
    virtual void clearKeys() = 0;
};

};

#endif
