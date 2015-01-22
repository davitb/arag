#ifndef __arag__AragException__
#define __arag__AragException__

#include <exception>
#include <string>

namespace arag
{
class AragException : public std::exception
{
public:
    AragException(const std::string& msg);
    
    virtual const char* what() const noexcept;
    
protected:
    std::string _msg;
};

inline AragException::AragException(const std::string& msg)
{
    _msg = msg;
}

inline const char* AragException::what() const noexcept
{
    return _msg.c_str();
}


#define DECLARE_EXCEPTION(CLASS_NAME)\
class CLASS_NAME : public AragException\
{\
public:\
    CLASS_NAME(const std::string& msg) : AragException(msg) {}\
};\

#define DECLARE_EXCEPTION_WITH_PREDEFINED_MESSAGE(CLASS_NAME, MSG)\
class CLASS_NAME : public AragException\
{\
public:\
    CLASS_NAME() : AragException(MSG) {}\
};\

#define DECLARE_EXCEPTION_WITH_EXTRA_MESSAGE(CLASS_NAME, EXTRA_MSG)\
class CLASS_NAME : public AragException\
{\
public:\
    CLASS_NAME(const std::string& msg) : AragException(EXTRA_MSG + msg) {}\
};\

DECLARE_EXCEPTION_WITH_PREDEFINED_MESSAGE(EInvalidRequest, "Failed to parse the request")
DECLARE_EXCEPTION_WITH_PREDEFINED_MESSAGE(EInvalidArgument, "Invalid argument")
DECLARE_EXCEPTION_WITH_EXTRA_MESSAGE(EInvalidCommand, "Invalid Command: ")
DECLARE_EXCEPTION_WITH_PREDEFINED_MESSAGE(EWrongKeyType, "Wrong key operation")
DECLARE_EXCEPTION_WITH_PREDEFINED_MESSAGE(EScriptFailed, "Lua script execution failed")
DECLARE_EXCEPTION_WITH_PREDEFINED_MESSAGE(EInvalidKey, "Key doesn't exist")
DECLARE_EXCEPTION_WITH_PREDEFINED_MESSAGE(EWrongDBIndex, "Index too big")
DECLARE_EXCEPTION_WITH_PREDEFINED_MESSAGE(EWrongSessionID, "Wrong sessionID")

};

#endif /* defined(__arag__AragException__) */
