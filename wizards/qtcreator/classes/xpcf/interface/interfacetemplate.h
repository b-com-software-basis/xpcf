%{Cpp:LicenseTemplate}\
#ifndef %{GUARD}
#define %{GUARD}


@if '%{Base}' && '%{Base}' !== 'None'
#include "%{Base}.h"
@else
#include <xpcf/api/IComponentIntrospect.h>
@endif

%{JS: Cpp.openNamespaces('%{Class}')}

@if '%{Base}' && '%{Base}' !== 'None'
class %{CN} : public virtual %{Base}
@else
class %{CN}: public virtual org::bcom::xpcf::IComponentIntrospect
@endif
{
public:
    virtual ~%{CN}() override = default;
};

%{JS: Cpp.closeNamespaces('%{Class}')}

template <> struct org::bcom::xpcf::InterfaceTraits<%{Class}>
{
    static constexpr const char * UUID = "%{UUID}";
    static constexpr const char * NAME = "%{CN}";
    static constexpr const char * DESCRIPTION = "%{CN} interface description";
};

#endif // %{GUARD}\
