
#include <map>

#include "xpcf/core/BaseException.h"
namespace org { namespace bcom { namespace xpcf {


std::map<XPCFErrorCode, std::string> xpcfBaseErrorCodeToMessageMap = {
    { XPCFErrorCode::_SUCCESS , "SUCCESS" },
    { XPCFErrorCode::_FAIL , "FAIL" },
    { XPCFErrorCode::_ERROR_INVALID_ARGUMENT , "ERROR: argument provided is invalid" },
    { XPCFErrorCode::_ERROR_DOMAIN , "ERROR: domain error " },
    { XPCFErrorCode::_ERROR_NOT_IMPLEMENTED , "ERROR: method is not implemented yet" },
    { XPCFErrorCode::_ERROR_NULL_POINTER , "ERROR: Null pointer argument provided" },
    { XPCFErrorCode::_ERROR_OUT_OF_RANGE , "ERROR: Out of range argument" },
    { XPCFErrorCode::_ERROR_TYPE , "ERROR: Unsupported type" },
    { XPCFErrorCode::_ERROR_OVERFLOW , "ERROR: overflow" },
    { XPCFErrorCode::_ERROR_UNDERFLOW , "ERROR: underflow" },
    { XPCFErrorCode::_ERROR_OUT_OF_MEMORY , "ERROR: out of memory" },
    { XPCFErrorCode::_ERROR_RANGE , "ERROR: range" },
    { XPCFErrorCode::_ERROR_SYSTEM , "ERROR: system" },
    { XPCFErrorCode::_ERROR_UNKNOWN , "ERROR: unknown" },
    { XPCFErrorCode::_ERROR_ACCESS_DENIED , "ERROR: access denied " },
    { XPCFErrorCode::_ERROR_TIMEOUT , "ERROR: timeout " },
    { XPCFErrorCode::_ERROR_ILLEGAL_STATE, "ERROR: illegal state " },
    { XPCFErrorCode::_ERROR_COMPONENT_UNKNOWN , "ERROR: unknown component " },
    { XPCFErrorCode::_ERROR_INTERFACE_UNKNOWN , "ERROR: unknown interface" },
    { XPCFErrorCode::_ERROR_MODULE_UNKNOWN , "ERROR: unknown module" },
    { XPCFErrorCode::_ERROR_MODULE_MISSINGXPCF_ENTRY , "ERROR: missing xpcf method(s) in the library (either one or several from getModuleUUID, getModuleName, getComponent ... )" },
    { XPCFErrorCode::_ERROR_INJECTION_FAILED , "ERROR: injection failed" },
    { XPCFErrorCode::_ERROR_INJECTABLE_UNKNOWN , "ERROR: injectable unknown" },
    { XPCFErrorCode::_ERROR_INJECTABLE_ALREADY_DECLARED , "ERROR: injectable already declared" },
    { XPCFErrorCode::_ERROR_INJECTABLE_NOBIND , "ERROR: no binding declared for injectable" },
};

Exception::Exception(XPCFErrorCode errCode):
    std::runtime_error(xpcfBaseErrorCodeToMessageMap[errCode]),m_errCode(errCode)
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
