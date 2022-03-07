%module XPCF_SampleComponent
%{
#include <xpcf/xpcf.h>
#include "IGuitar.h"
#include "IMusician.h"
#include "IHuman.h"
#include "IGuitarist.h"
%}

%include "swig.i"

%import (module="XPCF.Api") "XPCF_Api.i"

%pragma(csharp) moduleimports=%{
    using XPCF.Api;
%}

%typemap(csimports) SWIGTYPE
%{
    using XPCF.Api;
%}

%shared_ptr(IGuitar)
%shared_ptr(IElectricGuitar)
%shared_ptr(ITuner)
%shared_ptr(IMusician)
%shared_ptr(IHuman)
%shared_ptr(IGuitarist)

%include "IGuitar.h"
%include "IMusician.h"
%include "IHuman.h"
%include "IGuitarist.h"

XPCF_BIND_TO(IGuitar, IGuitar)
XPCF_BIND_TO(IElectricGuitar, IElectricGuitar)
XPCF_BIND_TO(IGuitarist, IGuitarist)
XPCF_BIND_TO(ITuner, ITuner)
XPCF_BIND_TO(IHuman, IHuman)
XPCF_BIND_TO(IMusician, IMusician)
