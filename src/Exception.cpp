
#include <map>

#include "xpcf/core/Exception.h"
namespace org { namespace bcom { namespace xpcf {

std::map<XPCFErrorCode, std::string> xpcfErrorCodeToMessageMap = {
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
{ XPCFErrorCode::_ERROR_COMPONENT_UNKNOWN , "ERROR: unknown component " },
{ XPCFErrorCode::_ERROR_INTERFACE_UNKNOWN , "ERROR: unknown interface" },
{ XPCFErrorCode::_ERROR_MODULE_UNKNOWN , "ERROR: unknown module" },
{ XPCFErrorCode::_ERROR_MODULE_NOGETCOMPONENT , "ERROR: no getcomponent method in module" }
};


Exception::Exception(XPCFErrorCode errCode):std::runtime_error(xpcfErrorCodeToMessageMap[errCode]),m_errCode(errCode)
{
}

Exception::Exception(const char *what, XPCFErrorCode errCode):std::runtime_error(what),m_errCode(errCode)
{
}

Exception::Exception(const std::string & what, XPCFErrorCode errCode):std::runtime_error(what),m_errCode(errCode)
{
}

Exception::~Exception()
{
}

AccessDeniedException::AccessDeniedException():Exception(XPCFErrorCode::_ERROR_ACCESS_DENIED)
{
}

AccessDeniedException::AccessDeniedException(const char * what):Exception(what, XPCFErrorCode::_ERROR_ACCESS_DENIED)
{
}

AccessDeniedException::AccessDeniedException(const std::string & what):Exception(what, XPCFErrorCode::_ERROR_ACCESS_DENIED)
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

ComponentNotFoundException::ComponentNotFoundException(const uuids::uuid & componentUUID)
    :ComponentNotFoundException("xpcf::ComponentNotFoundException: " + uuids::to_string(componentUUID))
{
}

ComponentNotFoundException::ComponentNotFoundException(const char * what):Exception(what, XPCFErrorCode::_ERROR_COMPONENT_UNKNOWN)
{
}

ComponentNotFoundException::ComponentNotFoundException(const std::string & what):Exception(what, XPCFErrorCode::_ERROR_COMPONENT_UNKNOWN)
{
}

InterfaceNotImplementedException::InterfaceNotImplementedException(const uuids::uuid & componentUUID,
                                                                   const uuids::uuid & interfaceUUID)
    :InterfaceNotImplementedException("Component " + uuids::to_string(componentUUID) +
                                      " doesn't implement interface " + uuids::to_string(interfaceUUID))
{
}

InterfaceNotImplementedException::InterfaceNotImplementedException(const char * what):Exception(what, XPCFErrorCode::_ERROR_INTERFACE_UNKNOWN)
{
}

InterfaceNotImplementedException::InterfaceNotImplementedException(const std::string & what):Exception(what, XPCFErrorCode::_ERROR_INTERFACE_UNKNOWN)
{
}



ModuleNotFoundException::ModuleNotFoundException(const uuids::uuid & moduleUUID)
    :ModuleNotFoundException("xpcf::ModuleNotFoundException: " + uuids::to_string(moduleUUID))
{
}

ModuleNotFoundException::ModuleNotFoundException(const char * what):Exception(what, XPCFErrorCode::_ERROR_MODULE_UNKNOWN)
{
}

ModuleNotFoundException::ModuleNotFoundException(const std::string & what):Exception(what, XPCFErrorCode::_ERROR_MODULE_UNKNOWN)
{
}

}}}
