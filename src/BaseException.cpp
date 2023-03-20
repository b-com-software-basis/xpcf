
#include <map>

#include "xpcf/core/BaseException.h"
namespace org { namespace bcom { namespace xpcf {

Exception::Exception(XPCFErrorCode errCode):
    std::runtime_error(xpcfErrorCodeToMessageMap[errCode]),m_errCode(errCode)
{
}

Exception::Exception(const char *what, XPCFErrorCode errCode):
    std::runtime_error(what),m_errCode(errCode)
{
}

Exception::Exception(const std::string & what, XPCFErrorCode errCode):
    std::runtime_error(what),m_errCode(errCode)
{
}

Exception::~Exception()
{
}

ConfigurationException::ConfigurationException(XPCFErrorCode errCode):
    Exception(errCode)
{
}
ConfigurationException::ConfigurationException(const char * what, XPCFErrorCode errCode):
    Exception(what, errCode)
{
}

ConfigurationException::ConfigurationException(const std::string & what, XPCFErrorCode errCode):
    Exception(what, errCode)
{
}

AccessDeniedException::AccessDeniedException():
    Exception(XPCFErrorCode::_ERROR_ACCESS_DENIED)
{
}

AccessDeniedException::AccessDeniedException(const char * what):
    Exception(what, XPCFErrorCode::_ERROR_ACCESS_DENIED)
{
}

AccessDeniedException::AccessDeniedException(const std::string & what):
    Exception(what, XPCFErrorCode::_ERROR_ACCESS_DENIED)
{
}

IllegalStateException::IllegalStateException():
    Exception(XPCFErrorCode::_ERROR_ACCESS_DENIED)
{
}

IllegalStateException::IllegalStateException(const char * what):
    Exception(what, XPCFErrorCode::_ERROR_ACCESS_DENIED)
{
}

IllegalStateException::IllegalStateException(const std::string & what):
    Exception(what, XPCFErrorCode::_ERROR_ILLEGAL_STATE)
{
}

NotImplementedException::NotImplementedException():Exception(XPCFErrorCode::_ERROR_NOT_IMPLEMENTED)
{
}

NotImplementedException::NotImplementedException(const char * what):Exception(what)
{
}

NotImplementedException::NotImplementedException(const std::string & what):Exception(what)
{
}

NullPointerException::NullPointerException():Exception(XPCFErrorCode::_ERROR_NULL_POINTER)
{
}

NullPointerException::NullPointerException(const char * what):
    Exception(what)
{
}

NullPointerException::NullPointerException(const std::string & what):
    Exception(what)
{
}

TimeoutException::TimeoutException():Exception(XPCFErrorCode::_ERROR_TIMEOUT)
{
}

TimeoutException::TimeoutException(const char * what):
    Exception(what)
{
}

TimeoutException::TimeoutException(const std::string & what):
    Exception(what)
{
}

RemotingException::RemotingException(const std::string & componentName,
                                     const std::string & rpcName,
                                     uint32_t status)
    :RemotingException("Error trying to call " + componentName + "::" + rpcName + " status=" + std::to_string(status))
{
}

RemotingException::RemotingException(const char * what):
    Exception(what, XPCFErrorCode::_ERROR_REMOTING)
{
}

RemotingException::RemotingException(const std::string & what):
    Exception(what, XPCFErrorCode::_ERROR_REMOTING)
{
}

}}}
