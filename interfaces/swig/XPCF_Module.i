%module xpcf_module
%{
#include "xpcf/module/ModuleFactory.h"
%}

%include "swig.i"

%import (module="XPCF.Core") "xpcf_core.i"
%import (module="XPCF.Component") "xpcf_Component.i"
%import (module="XPCF.Api") "xpcf_api.i"

%typemap(csimports) SWIGTYPE
%{
    using XPCF.Core;
    using XPCF.Component;
    using XPCF.Api;
%}

///

%include "xpcf/module/ModuleFactory.h"
