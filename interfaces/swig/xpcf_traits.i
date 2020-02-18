%module xpcf_traits
%{
#include "xpcf/component/ComponentTraits.h"
#include "xpcf/api/InterfaceTraits.h"
#include "xpcf/core/traits.h"
%}

%include "Swig.i"

%import (module="XPCF.Core") "xpcf_core.i"

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
