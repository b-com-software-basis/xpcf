%module XPCF_Traits
%{
#include "xpcf/component/ComponentTraits.h"
#include "xpcf/api/InterfaceTraits.h"
#include "xpcf/core/traits.h"
%}

%include "swig.i"

%import (module="XPCF.Core") "XPCF_Core.i"

%typemap(csimports) SWIGTYPE
%{
    using XPCF.Core;
%}

///

%include "xpcf/api/InterfaceTraits.h"

%include "xpcf/component/ComponentTraits.h"

//%include "xpcf/traits/InterfaceTraits.i"
//%include "xpcf/traits/ComponentTraits.i"
%include "xpcf/core/traits.h"
