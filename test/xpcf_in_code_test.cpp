/**
 * @copyright Copyright (c) 2015 All Right Reserved, B-com http://www.b-com.com/
 *
 * This file is subject to the B<>Com License.
 * All other rights reserved.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * @author Loïc Touraine
 *
 * @file
 * @brief description of file
 * @date 2015-09-18
 */

#include <stdio.h>
#include "IComponentManager.h"
#include "ComponentFactory.h"
#include "I0.h"
#include "I1.h"
#include "I2.h"
#include "C0.h"
#include <boost/uuid/uuid_io.hpp>
#include <boost/dll.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/function.hpp>
#include <boost/system/system_error.hpp>
#include <iostream>

// the UUID of the Implemntation Class C0

#define BOOST_TEST_MODULE test xpcf
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace std;

namespace xpcf  = org::bcom::xpcf;

#include <boost/uuid/uuid_generators.hpp>
boost::uuids::string_generator gen;
boost::uuids::uuid clsid_C0 = gen(C0::UUID );
boost::uuids::uuid iid_IComponentIntrospect= gen(xpcf::IComponentIntrospect::UUID );
boost::uuids::uuid iid_I0= gen(I0::UUID );
boost::uuids::uuid iid_I1= gen(I1::UUID );
boost::uuids::uuid iid_I2= gen(I2::UUID );

SRef<xpcf::IComponentManager> xpcfComponentManager;
SRef<xpcf::IComponentIntrospect> rIComponentIntrospect;
SRef<I0> rI0;
SRef<I1> rI1;
SRef<I2> rI2;

SRef<I1> testComponentForwarding(SRef<I0> rI) {
    BOOST_TEST_MESSAGE("TC::testComponentForwarding::BEGIN");
    SRef<I1> rII1;
    rI->I0_opname();
    rI->I0_opname2();
    rI->I0_opname4();
    rI->I0_opname3();
    rI->queryInterface(iid_I1,rII1);
    BOOST_TEST_MESSAGE("TC::testComponentForwarding::END");
    return rII1;
}

SRef<I1> testRefComponentForwarding(const SRef<I0> & rI) {
    BOOST_TEST_MESSAGE("TC::testRefComponentForwarding::BEGIN");
    SRef<I1> rII1;
    rI->I0_opname();
    rI->I0_opname2();
    rI->I0_opname4();
    rI->I0_opname3();
    rI->queryInterface(iid_I1,rII1);
    BOOST_TEST_MESSAGE("TC::testRefComponentForwarding::END");
    return rII1;
}
struct XpcfFixture {
    XpcfFixture() { xpcfComponentManager = xpcf::getComponentManagerInstance(); BOOST_TEST_MESSAGE( "setup fixture" ); }
    ~XpcfFixture()         { BOOST_TEST_MESSAGE( "teardown fixture" ); }

    SRef<xpcf::IComponentManager> xpcfComponentManager;
};

BOOST_AUTO_TEST_SUITE( test_in_code_creation )

BOOST_AUTO_TEST_CASE( test_fixture )
{
    XpcfFixture fixture;
    SRef<xpcf::IComponentManager> localxpcfComponentManager;
    BOOST_TEST(fixture.xpcfComponentManager );
}

BOOST_FIXTURE_TEST_CASE( test_container_introspection,XpcfFixture,* boost::unit_test::depends_on("test_in_code_creation/test_fixture"))
{
    BOOST_TEST(xpcfComponentManager );
    const char * bcomDevRootPath = getenv("BCOMDEVROOT");
    std::string containerPath = getenv("BCOMDEVROOT");
    containerPath += "/bcomBuild/xpcfSampleComponent/1.0.0/lib/x86_64/shared/debug/libxpcfSampleComponent.dylib";
    SPtr<xpcf::ContainerMetadata> containerInfos = xpcfComponentManager->introspectContainer(containerPath.c_str());
    BOOST_TEST_REQUIRE(containerInfos," container introspection failed for path"<<containerPath);
    BOOST_TEST_MESSAGE("testContainerIntrospection::Introspecting container UUID: "<<boost::uuids::to_string(containerInfos->getUUID())<<" Name: "<<containerInfos->getDescription());
    BOOST_TEST_MESSAGE("testContainerIntrospection::ComponentList :");
    for (unsigned long i = 0 ; i<containerInfos->getNbComponents() ; i++) {
        BOOST_TEST_MESSAGE("=> "<<boost::uuids::to_string(containerInfos->getComponent(i)));
    }
}

BOOST_AUTO_TEST_CASE( test_component_creation)
{
    SRef<I0> lrI0;
    SRef<I1> lrI1;
    {
        //SRef(C0) rBase = sptrnms::make_shared<C0>();
        {
            //sptrnms::shared_ptr<IComponentIntrospect> rII(rBase, dynamic_cast<IComponentIntrospect*>(rBase.get()));
            SRef<I0> lrI0b;
            SRef<xpcf::IComponentIntrospect> lrIComponentIntrospect;

            SRef<I2> lrI2;

            xpcf::ComponentFactory::createComponent<C0>(lrIComponentIntrospect);
            BOOST_TEST(lrIComponentIntrospect," Failed creating component C0");
            lrIComponentIntrospect->queryInterface(iid_I0,lrI0b);
            //try {
            lrIComponentIntrospect->queryInterface(iid_I0,lrI2);
            //}
            if (!lrI2)
                //catch(const sptrnms::bad_any_cast &)
            {
                BOOST_TEST_MESSAGE("TC::Introspection OK QueryInterface(iid_I0, lrI2) throwed bad_any_cast exception as expected");
            }
            lrI0 = lrI0b;
            //    rII->queryInterface(iid_I0,rI0);
            BOOST_TEST_MESSAGE("TC::Introspection OK Calling some component methods of I0");
            lrI0b->I0_opname();
            lrI0b->I0_opname2();
            lrI0b->I0_opname4();
            lrI0b->I0_opname3();
            lrI0b->queryInterface(iid_I1,lrI1);
            BOOST_TEST_MESSAGE("TC::Introspection OK QueryInterface(iid_I1, lrI1);");
            // test it
            //
            BOOST_TEST_MESSAGE("TC::Introspection OK Calling some component methods I1");
            lrI1->I1_opname();
            lrI1->I1_opname2();
            xpcf::ComponentFactory::createComponent<C0>(iid_I1,lrI1);
            lrI1->I1_opname3();
            lrI1->I1_opname4();
        }
    }
    lrI1->I1_opname4();
    BOOST_TEST_MESSAGE("TC::Introspection testComponentForwarding::BEGIN");
    SRef<I1> lrI1b = testComponentForwarding(lrI0);
    BOOST_TEST_MESSAGE("TC::Introspection testComponentForwarding::END");
    lrI1b->I1_opname3();
    BOOST_TEST_MESSAGE("TC::Introspection testRefComponentForwarding::BEGIN");
    SRef<I1> lrI1b1 = testRefComponentForwarding(lrI0);
    BOOST_TEST_MESSAGE("TC::Introspection testRefComponentForwarding::END");
    lrI1b1->I1_opname2();
    BOOST_TEST_MESSAGE("TC::Introspection Ended");
}

BOOST_AUTO_TEST_SUITE_END()


