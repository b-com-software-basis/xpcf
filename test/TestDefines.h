#ifndef TESTDEFINES_H
#define TESTDEFINES_H

#include "xpcfSampleComponent_traits.h"
#include "xpcf/xpcf.h"
#include "IHuman.h"
#include "IMusician.h"
#include "IGuitarist.h"

namespace xpcf  = org::bcom::xpcf;

#include <boost/uuid/uuid_generators.hpp>

extern xpcf::uuids::uuid clsid_HumanMusician;
extern xpcf::uuids::uuid iid_IComponentIntrospect;
extern xpcf::uuids::uuid iid_IHuman;
extern xpcf::uuids::uuid iid_IMusician;
extern xpcf::uuids::uuid iid_IGuitarist;

class fakeComponent;

template <> struct xpcf::ComponentTraits<fakeComponent>
{
    static constexpr const char * UUID = "619CB6E1-396E-4AFD-9951-287A30FA76B6";
    static constexpr const char * DESCRIPTION = "fake component";
};

struct XpcfFixture {
    XpcfFixture() { xpcfComponentManager = xpcf::getComponentManagerInstance(); BOOST_TEST_MESSAGE( "setup fixture" ); }
    ~XpcfFixture() { BOOST_TEST_MESSAGE( "teardown fixture" ); }

    SRef<xpcf::IComponentManager> xpcfComponentManager;
};


#endif // TESTDEFINES_H
