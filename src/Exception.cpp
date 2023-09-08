
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


UUIDNotFoundException::UUIDNotFoundException(const uuids::uuid & uuid)
    :UUIDNotFoundException("xpcf::UUIDNotFoundException: " + uuids::to_string(uuid))
{
}

UUIDNotFoundException::UUIDNotFoundException(const char * what, XPCFErrorCode errCode):
    Exception(what, errCode)
{
}

UUIDNotFoundException::UUIDNotFoundException(const std::string & what, XPCFErrorCode errCode):
    Exception(what, errCode)
{
}

ComponentNotFoundException::ComponentNotFoundException(const uuids::uuid & componentUUID)
    :ComponentNotFoundException("xpcf::ComponentNotFoundException: " + uuids::to_string(componentUUID))
{
}

ComponentNotFoundException::ComponentNotFoundException(const char * what):
    UUIDNotFoundException(what, XPCFErrorCode::_ERROR_COMPONENT_UNKNOWN)
{
}

ComponentNotFoundException::ComponentNotFoundException(const std::string & what):
    UUIDNotFoundException(what, XPCFErrorCode::_ERROR_COMPONENT_UNKNOWN)
{
}

InterfaceNotImplementedException::InterfaceNotImplementedException(const uuids::uuid & componentUUID,
                                                                   const uuids::uuid & interfaceUUID)
    :InterfaceNotImplementedException("Component " + uuids::to_string(componentUUID) +
                                      " doesn't implement interface " + uuids::to_string(interfaceUUID))
{
}

InterfaceNotImplementedException::InterfaceNotImplementedException(const char * what):
    Exception(what, XPCFErrorCode::_ERROR_INTERFACE_UNKNOWN)
{
}

InterfaceNotImplementedException::InterfaceNotImplementedException(const std::string & what):
    Exception(what, XPCFErrorCode::_ERROR_INTERFACE_UNKNOWN)
{
}

ModuleException::ModuleException():Exception(XPCFErrorCode::_ERROR_MODULE_MISSINGXPCF_ENTRY)
{
}

ModuleException::ModuleException(const char * what, XPCFErrorCode errCode):
    Exception(what, errCode)
{
}

ModuleException::ModuleException(const std::string & what, XPCFErrorCode errCode):
    Exception(what, errCode)
{
}

ModuleNotFoundException::ModuleNotFoundException(const uuids::uuid & moduleUUID)
    :ModuleNotFoundException("xpcf::ModuleNotFoundException: " + uuids::to_string(moduleUUID))
{
}

ModuleNotFoundException::ModuleNotFoundException(const char * what):
    UUIDNotFoundException(what, XPCFErrorCode::_ERROR_MODULE_UNKNOWN)
{
}

ModuleNotFoundException::ModuleNotFoundException(const std::string & what):
    UUIDNotFoundException(what, XPCFErrorCode::_ERROR_MODULE_UNKNOWN)
{
}

std::string buildExceptionMessage(const SRef<InjectableMetadata> injectableInfo, XPCFErrorCode errCode,  const char * what)
{
    std::string message;
#ifdef XPCFDEBUG
    if (xpcfErrorCodeToMessageMap.find(errCode) != xpcfErrorCodeToMessageMap.end()) {
#endif
        message = xpcfErrorCodeToMessageMap[errCode];
        message += "=>\n";
#ifdef XPCFDEBUG
    }
#endif
    message = "uuid= " + uuids::to_string(injectableInfo->getUuid());
    if (injectableInfo->isNamed()) {
         message += "\nname=";
         message += injectableInfo->getName();
    }
    message += "\n";
    message += what;
    return message;
}

InjectionException::InjectionException(const SRef<InjectableMetadata> injectableInfo, XPCFErrorCode errCode, const char * what ):
    InjectionException(buildExceptionMessage(injectableInfo, errCode, what), errCode)
{
}

InjectionException::InjectionException(const char * what, XPCFErrorCode errCode):
    UUIDNotFoundException(what, errCode)
{
}

InjectionException::InjectionException(const std::string & what, XPCFErrorCode errCode):UUIDNotFoundException(what, errCode)
{
}


InjectableNotFoundException::InjectableNotFoundException(const SRef<InjectableMetadata> injectableInfo):
    InjectionException(injectableInfo,XPCFErrorCode::_ERROR_INJECTABLE_NOBIND)
{
}

InjectableNotFoundException::InjectableNotFoundException(const char * what):
    InjectionException(what, XPCFErrorCode::_ERROR_INJECTABLE_NOBIND)
{
}

InjectableNotFoundException::InjectableNotFoundException(const std::string & what):
    InjectionException(what, XPCFErrorCode::_ERROR_INJECTABLE_NOBIND)
{
}



InjectableDeclarationException::InjectableDeclarationException(const SRef<InjectableMetadata> injectableInfo):
    InjectionException(injectableInfo,XPCFErrorCode::_ERROR_INJECTABLE_ALREADY_DECLARED)
{
}

InjectableDeclarationException::InjectableDeclarationException(const char * what):
    InjectionException(what, XPCFErrorCode::_ERROR_INJECTABLE_ALREADY_DECLARED)
{
}

InjectableDeclarationException::InjectableDeclarationException(const std::string & what):
    InjectionException(what, XPCFErrorCode::_ERROR_INJECTABLE_ALREADY_DECLARED)
{
}

}}}
