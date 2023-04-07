#ifndef ORG_BCOM_XPCF_EXCEPTION_H
#define ORG_BCOM_XPCF_EXCEPTION_H

#include "xpcf/core/BaseException.h"
#include "xpcf/core/uuid.h"
#include "xpcf/api/InjectableMetadata.h"

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API UUIDNotFoundException : public Exception {
protected:
    UUIDNotFoundException(const uuids::uuid & uuid);
    UUIDNotFoundException(const char * what, XPCFErrorCode errCode = XPCFErrorCode::_FAIL);
    UUIDNotFoundException(const std::string & what, XPCFErrorCode errCode = XPCFErrorCode::_FAIL);
    virtual ~UUIDNotFoundException() = default;
};

class XPCF_EXPORT_API ComponentNotFoundException : public UUIDNotFoundException {
public:
    ComponentNotFoundException(const uuids::uuid & componentUUID);
    ComponentNotFoundException(const char * what);
    ComponentNotFoundException(const std::string & what);
    virtual ~ComponentNotFoundException() = default;
};

class XPCF_EXPORT_API InterfaceNotImplementedException : public Exception {
public:
    InterfaceNotImplementedException(const uuids::uuid & componentUUID, const uuids::uuid & interfaceUUID);
    InterfaceNotImplementedException(const char * what);
    InterfaceNotImplementedException(const std::string & what);
    virtual ~InterfaceNotImplementedException() = default;
};

// TODO : provide modulemetadata to exception ???
class XPCF_EXPORT_API ModuleException : public Exception {
public:
    ModuleException();
    ModuleException(const char * what, XPCFErrorCode errCode = XPCFErrorCode::_FAIL);
    ModuleException(const std::string & what, XPCFErrorCode errCode = XPCFErrorCode::_FAIL);
    virtual ~ModuleException() = default;
};

class XPCF_EXPORT_API ModuleNotFoundException : public UUIDNotFoundException {
public:
    ModuleNotFoundException(const uuids::uuid & moduleUUID);
    ModuleNotFoundException(const char * what);
    ModuleNotFoundException(const std::string & what);
    virtual ~ModuleNotFoundException() = default;
};

class XPCF_EXPORT_API InjectionException : public UUIDNotFoundException {
public:
    InjectionException(const SRef<InjectableMetadata> injectableInfo, XPCFErrorCode errCode = XPCFErrorCode::_ERROR_INJECTION_FAILED, const char * what = "" );
    InjectionException(const char * what, XPCFErrorCode errCode = XPCFErrorCode::_FAIL);
    InjectionException(const std::string & what, XPCFErrorCode errCode = XPCFErrorCode::_FAIL);
    virtual ~InjectionException() = default;
};

class XPCF_EXPORT_API InjectableNotFoundException : public InjectionException {
public:
    InjectableNotFoundException(const SRef<InjectableMetadata> injectableInfo);
    InjectableNotFoundException(const char * what);
    InjectableNotFoundException(const std::string & what);
    virtual ~InjectableNotFoundException() = default;
};

class XPCF_EXPORT_API InjectableDeclarationException : public InjectionException {
public:
    InjectableDeclarationException(const SRef<InjectableMetadata> injectableInfo);
    InjectableDeclarationException(const char * what);
    InjectableDeclarationException(const std::string & what);
    virtual ~InjectableDeclarationException() = default;
};

}}}

#endif
