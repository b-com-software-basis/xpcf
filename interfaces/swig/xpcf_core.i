%module xpcf_core
%{
#include "xpcf/core/helpers.h"
#include "xpcf/core/refs.h"
#include "xpcf/core/Result.h"
#include "xpcf/core/uuid.h"
#include "xpcf/core/traits.h"
#include "xpcf/xpcf_api_define.h"
#include "xpcf/core/XPCFErrorCode.h"
%}

%include "Swig.i"

///

%include "xpcf/core/helpers.h"

%include "xpcf/xpcf_api_define.h"

%rename (UUID) boost::uuids::uuid;
namespace boost {namespace uuids {class uuid{};}}
%csmethodmodifiers boost::uuids::uuid::ToString %{public override%};
%extend boost::uuids::uuid
{
	%proxycode
	%{
		public static implicit operator UUID(string uuidString) {return new UUID(uuidString);}
		public static implicit operator string(UUID uuid) {return uuid.ToString();}
	%}
	uuid(const char* uuidString) {return new boost::uuids::uuid(org::bcom::xpcf::toUUID(uuidString));}
	const std::string ToString() {return boost::uuids::to_string(*self);}
}
%ignore org::bcom::xpcf::toUUID(const char * uuidString);
%ignore org::bcom::xpcf::toUUID(const std::string & uuidString);
%include "xpcf/core/uuid.h"

%include "xpcf/core/traits.h"

namespace boost {namespace movelib {}}
%include "xpcf/core/refs.h"
#define SRef boost::shared_ptr
#define SPtr boost::shared_ptr

%include "xpcf/core/XPCFErrorCode.h"

//#include "xpcf/core/XPCFErrorCode.i"
%include "xpcf/core/Result.h"