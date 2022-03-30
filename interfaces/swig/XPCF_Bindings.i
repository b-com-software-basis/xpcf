//This module is the C# bindTo entry point for the whole xpcf and modules interfaces wrapping
%module XPCF_Bindings
%{
#include <xpcf/xpcf.h>
#include "xpcf/api/IComponentIntrospect.h"
#include "xpcf/api/IFactory.h"
#include "xpcf/api/IAliasManager.h"
#include "xpcf/api/IRegistryManager.h"
#include "xpcf/api/IComponentManager.h"
#include "xpcf/api/IConfigurable.h"
#include "xpcf/api/IInjectable.h"
#include "IGuitar.h"
#include "IMusician.h"
#include "IHuman.h"
#include "IGuitarist.h"
%}

%include "swig.i"

%import (module="XPCF.Api") "XPCF_Api.i"
%import (module="XPCF.SampleComponent") "XPCF_SampleComponent.i"

%pragma(csharp) moduleimports=%{
    using XPCF.Api;
    using XPCF.SampleComponent;
%}

%typemap(csimports) SWIGTYPE
%{
    using XPCF.Api;
    using XPCF.SampleComponent;
%}

%define XPCF_BIND_TO(NAME, TYPE)
%template(bindTo_ ## NAME) org::bcom::xpcf::bindTo<TYPE>;
%template(resolve_ ## NAME) org::bcom::xpcf::resolve<TYPE>;
%template(resolveAll_ ## NAME) org::bcom::xpcf::resolveAll<TYPE>;
%enddef

// Base XPCF interfaces bindTo wrapping
XPCF_BIND_TO(IAliasManager, org::bcom::xpcf::IAliasManager)
XPCF_BIND_TO(IRegistryManager, org::bcom::xpcf::IRegistryManager)
XPCF_BIND_TO(IInjectable, org::bcom::xpcf::IInjectable)
XPCF_BIND_TO(IConfigurable, org::bcom::xpcf::IConfigurable)
XPCF_BIND_TO(IInjectable, org::bcom::xpcf::IConfigurable)
// Add dedicated other framework interfaces bindTo wrapping below
// sample wrapping for xpcfSampleComponent interfaces
XPCF_BIND_TO(IGuitar, IGuitar)
XPCF_BIND_TO(IElectricGuitar, IElectricGuitar)
XPCF_BIND_TO(IGuitarist, IGuitarist)
XPCF_BIND_TO(ITuner, ITuner)
XPCF_BIND_TO(IHuman, IHuman)
XPCF_BIND_TO(IMusician, IMusician)
// xpcfSampleComponent sample wrapping end
