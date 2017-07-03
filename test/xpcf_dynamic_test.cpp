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
#include <boost/uuid/uuid_io.hpp>
#include <boost/dll.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/function.hpp>
#include <boost/system/system_error.hpp>
#include <iostream>

#define BOOST_TEST_MODULE test dynamic xpcf
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

using namespace std;

namespace xpcf  = org::bcom::xpcf;

#include <boost/uuid/uuid_generators.hpp>
xpcf::uuids::string_generator gen;
xpcf::uuids::uuid clsid_C0 = gen("ae9d10bb-4bd1-462b-8c01-43f1815c6ae0" );
xpcf::uuids::uuid iid_IComponentIntrospect= gen(xpcf::IComponentIntrospect::UUID );
xpcf::uuids::uuid iid_I0= gen(I0::UUID );
xpcf::uuids::uuid iid_I1= gen(I1::UUID );
xpcf::uuids::uuid iid_I2= gen(I2::UUID );

SRef<xpcf::IComponentManager> xpcfComponentManager;
SRef<xpcf::IComponentIntrospect> rIComponentIntrospect;
SRef<I0> rI0;
SRef<I1> rI1;
SRef<I2> rI2;

/*------------------------------*/
void TCDLL()
{
    try {
        boost::dll::shared_library shlib;
        //shlib.load(boost::dll::program_location());
        shlib.load("/Users/ltouraine/workspace/test-lto/lto_components/XPCF_framework/xpcf/build-test-Desktop_Qt_5_4_2_clang_64bit-Debug/test.app/Contents/MacOS/test");
        if (shlib.has("my_test_func")) {
            boost::function<void(const char * )> testFunc = boost::dll::import<void(const char * )>(boost::dll::program_location(), "my_test_func"
                                                                                                    );

            if (testFunc) {
                testFunc("message");
            }
        }

    }
    catch (boost::system::system_error & e) {
        BOOST_TEST_MESSAGE("Got exception");

    }


}

struct XpcfFixture {
    XpcfFixture() { xpcfComponentManager = xpcf::getComponentManagerInstance(); BOOST_TEST_MESSAGE( "setup fixture" ); }
    ~XpcfFixture()         { BOOST_TEST_MESSAGE( "teardown fixture" ); }

    SRef<xpcf::IComponentManager> xpcfComponentManager;
};

BOOST_AUTO_TEST_SUITE( test_library_component_metadata )

BOOST_AUTO_TEST_CASE( test_fixture )
{
    XpcfFixture fixture;
    SRef<xpcf::IComponentManager> localxpcfComponentManager;
    BOOST_TEST(fixture.xpcfComponentManager );
}


BOOST_FIXTURE_TEST_CASE( test_load_library,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_fixture"))
{
    BOOST_TEST(xpcfComponentManager->load() == 1, " failed to load library from "<<getenv("XPCF_REGISTRY_PATH") );
}


BOOST_FIXTURE_TEST_CASE( test_read_interface,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    /* finding interfaces using XPCF_REGISTRY class.
        ** getting  the interface Name using XPCF_Interface class
        */
    xpcfComponentManager->load();
    SPtr<xpcf::InterfaceMetadata> pI;

    pI=xpcfComponentManager->findInterfaceMetadata(iid_I0);

    BOOST_TEST_REQUIRE(pI,"ERROR Cannot find interface iid_I0 !");
    BOOST_TEST_MESSAGE("TC2-1 OK. Interface iid_I0 found! its Description is:"<<pI->getDescription());
}

BOOST_FIXTURE_TEST_CASE( test_read_components,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    xpcfComponentManager->load();
    /*
    ** 1)(using XPCF_Registry) getting number of component classes registrated
    ** 2)(using XPCF_Registry) getting a the component index=0 to obtain its (SPtr_XPCF_ComponentClass)
    ** 3)(using SPtr_XPCF_ComponentClass) obtaining the component class name (char*)
    ** 4)(using XPCF_Registry) finding a component classs from its UUID (clsid_C0) to get its (SPtr_XPCF_ComponentClass)
    ** 5)(using XPCF_Registry) finding a component classs from its name( help string) UUID (clsid_C0) to obtain its (xpcf::uuids::uuid)
    */
    BOOST_TEST_MESSAGE("TC3-1 OK. "<<xpcfComponentManager->getNbComponentMetadatas()<<" Component class(es) found in registry");

    SPtr<xpcf::ComponentMetadata> poCC;

    poCC= xpcfComponentManager->getComponentMetadata(0);
    BOOST_TEST_REQUIRE(poCC,"ERROR Cannot get ComponentMetadata for component index 0 !");
    BOOST_TEST_MESSAGE("TC3-2 OK. Component class index 0 found:"<<poCC->getDescription());


    poCC= xpcfComponentManager->findComponentMetadata(clsid_C0);
    BOOST_TEST_REQUIRE(poCC,"ERROR Cannot get ComponentMetadata for component ID "<<clsid_C0<<" !");
    BOOST_TEST_MESSAGE("TC3-4 OK. Component Class clsid_C0 found! Its Description is:"<<poCC->getDescription());
}

BOOST_FIXTURE_TEST_CASE( test_read_component,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    xpcfComponentManager->load();
    /*
    ** 1)(using the SPtr_XPCF_ComponentClass of clsid_C0)
    **    getting the path name of the Container library of the component Class
    ** 2) (using the SPtr_XPCF_ComponentClass of clsid_C0)
    **	  getting number of interfaces implemented by the component class "clsid_C0"
    ** 3) (using the SPtr_XPCF_ComponentClass of clsid_C0)
    **     Obtaining then re-setting the component Class Name(help string)
    **
    **
    */

    SPtr<xpcf::ComponentMetadata> poCC;
    poCC= xpcfComponentManager->findComponentMetadata(clsid_C0);

    BOOST_TEST_MESSAGE("TC4-1 OK. Container UUID: "<<poCC->getContainerUUID());

    int nbI=poCC->getNbInterfaces();
    BOOST_TEST_MESSAGE("TC4-2 OK. "<<nbI<<" interfaces found in component Class");


    /*BOOST_TEST_MESSAGE("TC4-X OK. interface "<<poCC->GetInterface(nbI-1)<<"found in component Class" );*/

}

BOOST_FIXTURE_TEST_CASE( test_read_interfaces,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    xpcfComponentManager->load();
    /*
    ** 1) using XPCF_Registry
    **    getting the number of implemented interfaces of any components declared in the SCP registry file.
    ** 2) Getting pointers (SPtr_XPCF_Interface) on all implemented interfaces of any components declared in the SCP registry file.
    **     Then with these pointers we  obtain the name (help string) of each interface.
    **/

    SPtr<xpcf::InterfaceMetadata> poCI;
    int nbI = xpcfComponentManager->getNbInterfaces();
    BOOST_TEST_MESSAGE("TC5-1 OK. "<<nbI<<" interfaces found the registry s");
    for(int ii=0; ii<nbI; ii++)
    {
        poCI= xpcfComponentManager->getInterfaceMetadata(ii);
        BOOST_TEST_REQUIRE(poCI,"ERROR::getInterfaceMetadata failed for interface index "<<ii<<" !");
        BOOST_TEST_MESSAGE("TC5-2-"<<ii<<" OK. Interface Description: "<<poCI->getDescription ());
    }

}
/*------------------------------*/
BOOST_AUTO_TEST_CASE( test_shared_exception)
{

    /** 1) Declare a SPtr_XPCF_Interface (poCI2) then try to use it before
 ***    having setting it to a valid value.
 *** 2) using the XPCF_Exception reference "E" we obtain the error message.
 ***
 **/

    SPtr<xpcf::InterfaceMetadata> poCI2;
    /*       try
    {
        //should never occur.
                    BOOST_TEST_MESSAGE("TC6 ERROR. Interface Description: "<<poCI2->getDescription());

    }
    catch(XPCF_NullObject * E)//WARNING : exception created in which scope ?? should it be sharedrefed ?
    {
        BOOST_TEST_MESSAGE("TC6-1 OK. testing exception. -Exception handled!- poCI2=null ");
        BOOST_TEST_MESSAGE("TC6-2 OK. E.GetErrorMessage():\t"<<E->GetErrorMessage());
    }

    */

}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE( test_library_component_creation )

BOOST_FIXTURE_TEST_CASE( test_component_creation,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    xpcfComponentManager->load() ;
    // create a component
    //
    int res=0;
    res=   xpcfComponentManager->createComponent(clsid_C0, rIComponentIntrospect);
    BOOST_TEST_REQUIRE(res == XPCF_OK, "ERROR:CreateInstance(clsid_C0, iid_I0 ,rI0)");
    rIComponentIntrospect->queryInterface(iid_I0,rI0);
    BOOST_TEST_REQUIRE(rI0, "ERROR:queryInterface(iid_I0,rI0)");
    BOOST_TEST_MESSAGE("TC1 OK. CreateInstance(clsid_C0, iid_I0 ,rI0)");
}

BOOST_FIXTURE_TEST_CASE( test_component_instropection,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    xpcfComponentManager->load() ;
    /**
    *** 1) from a reference of the implementation class I0 (SRef<I0>) we obtain
    ***    the number of interfaces implemnted by the component class is(I0) belonging to.
    *** 2) from a reference of the implementation class I0 (SRef<I0>) we fill an
    ***    array (of xpcf::uuids::uuid) up interfaces uuids.
    *** 3) with the interface UUIDs we enumerate each interface help string.
    **/
    int res=0;
    res=   xpcfComponentManager->createComponent(clsid_C0, rIComponentIntrospect);
    rIComponentIntrospect->queryInterface(iid_I0,rI0);

    int nbI = rI0->getNbInterfaces();
    BOOST_TEST_MESSAGE("TC7-1 OK. "<<nbI<<" Interface(s) found from I0:");

    xpcf::uuids::uuid  puuid10[100] ;
    rI0->getInterfaces(&puuid10[0]);
    BOOST_TEST_MESSAGE("TC7-2 OK. filling xpcf::uuids::uuid  puuid10[100]  array of Interface uuids from I0:");

    for( int ii=0; ii<nbI; ii++)
    {
        BOOST_TEST_MESSAGE("TC7-3-["<<ii<<"] OK. Interface: "<<xpcf::uuids::to_string(puuid10[ii]).c_str());

    }
}

/*------------------------------*/
BOOST_FIXTURE_TEST_CASE( test_component_invocation,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    xpcfComponentManager->load() ;
    int res=0;
    res=   xpcfComponentManager->createComponent(clsid_C0, rIComponentIntrospect);
    rIComponentIntrospect->queryInterface(iid_I0,rI0);

    BOOST_TEST_MESSAGE("TC8-1 OK Calling some component methods of I0");
    rI0->I0_opname();
    rI0->I0_opname2();
    rI0->I0_opname4();


    // look for another interface
    //
    //rI0->queryInterface(iid_I1, rI1);
    rIComponentIntrospect->queryInterface(iid_I1,rI1);
    BOOST_TEST_MESSAGE("TC8-2 OK QueryInterface(iid_I1, rI1);");
    // test it
    //
    BOOST_TEST_MESSAGE("TC8-3 OK Calling some component methods I1");
    rI1->I1_opname();
    rI1->I1_opname2();
    rI1->I1_opname3();
    rI1->I1_opname4();

}

BOOST_AUTO_TEST_SUITE_END()


