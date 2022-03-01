%module xpcf_properties
%{
#include "xpcf/properties/IProperty.h"
#include "xpcf/properties/IPropertyMap.h"
%}

%include "swig.i"

%import (module="XPCF.Core") "xpcf_core.i"
%import (module="XPCF.Collection") "xpcf_collection.i"

%typemap(csimports) SWIGTYPE
%{
    using XPCF.Core;
    using XPCF.Collection;
%}

///

%shared_ptr(org::bcom::xpcf::IProperty)
%shared_ptr(org::bcom::xpcf::IPropertyMap)

///

%include "xpcf/properties/IProperty.h"

//%include "xpcf/properties/IProperty.i"
//ignore wrapping methods
%rename("$ignore", regextarget=1, fullname=1) "org::bcom::xpcf::IPropertyMap::wrap.*$";
%include "xpcf/properties/IPropertyMap.h"
