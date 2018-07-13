/**
 * @copyright Copyright (c) 2017 B-com http://www.b-com.com/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author Loïc Touraine
 *
 * @file
 * @brief description of file
 * @date 2018-07-02
 */

#include <stdio.h>
#include <boost/uuid/uuid_io.hpp>
#include <boost/dll.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/function.hpp>
#include <boost/system/system_error.hpp>
#include <iostream>
#include "xpcf/threading/BaseTask.h"

#define BOOST_TEST_MODULE test dynamic xpcf
#include <boost/test/unit_test.hpp>

#include "TestDefines.h"

xpcf::uuids::string_generator gen;
xpcf::uuids::uuid clsid_C0 = gen("ae9d10bb-4bd1-462b-8c01-43f1815c6ae0" );
xpcf::uuids::uuid iid_IComponentIntrospect= xpcf::toUUID<xpcf::IComponentIntrospect>();
xpcf::uuids::uuid iid_I0= xpcf::toUUID<I0>();
xpcf::uuids::uuid iid_I1= xpcf::toUUID<I1>();
xpcf::uuids::uuid iid_I2= xpcf::toUUID<I2>();

using namespace std;

//TODO : unittests, exception catching
/*------------------------------*/
void TCDLL()
{
    try {
        boost::dll::shared_library shlib;
        //shlib.load(boost::dll::program_location());
        shlib.load("test");
        if (shlib.has("my_test_func")) {
            boost::function<void(const char * )> testFunc = boost::dll::import<void(const char * )>(boost::dll::program_location(), "my_test_func");

            if (testFunc) {
                testFunc("message");
            }
        }

    }
    catch (boost::system::system_error & e) {
        BOOST_TEST_MESSAGE("Got exception");

    }
}



BOOST_AUTO_TEST_SUITE( test_library_component_metadata )

BOOST_AUTO_TEST_CASE( test_fixture )
{
    XpcfFixture fixture;
    SRef<xpcf::IComponentManager> localxpcfComponentManager;
    BOOST_TEST(fixture.xpcfComponentManager );
}

BOOST_AUTO_TEST_CASE( test_threading)
{
    xpcf::SharedFifo<int> inFifo;
    xpcf::SharedFifo<std::string> outFifo;
    std::function<void(void)> func = [&inFifo,&outFifo](){
        int valueIn = inFifo.pop();
        valueIn += 2;
        outFifo.push(to_string(valueIn));
    };
    xpcf::DelegateTask task(func);
    task.start();
    inFifo.push(2);
    inFifo.push(3);
    BOOST_TEST_MESSAGE("THREADING OK. OutFifo first element is:"<<outFifo.pop());
    BOOST_TEST_MESSAGE("THREADING OK. OutFifo second element is:"<<outFifo.pop());
}

BOOST_FIXTURE_TEST_CASE( test_load_library,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_fixture"))
{
    BOOST_TEST(xpcfComponentManager->load("$BCOMDEVROOT/.xpcf/test/xpcf_registry_test.xml",false) == 0, " failed to load library from "<<getenv("BCOMDEVROOT")<<"/.xpcf/test/xpcf_registry_test.xml" );
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
    **    getting the path name of the Module library of the component Class
    ** 2) (using the SPtr_XPCF_ComponentClass of clsid_C0)
    **	  getting number of interfaces implemented by the component class "clsid_C0"
    ** 3) (using the SPtr_XPCF_ComponentClass of clsid_C0)
    **     Obtaining then re-setting the component Class Name(help string)
    **
    **
    */

    SPtr<xpcf::ComponentMetadata> poCC;
    poCC= xpcfComponentManager->findComponentMetadata(clsid_C0);

    BOOST_TEST_MESSAGE("TC4-1 OK. Module UUID: "<<poCC->getModuleUUID());

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
    //xpcf::uuids::uuid * leakedUUID = new xpcf::uuids::uuid();
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

void displayParameter(SRef<xpcf::IProperty> p)
{
    BOOST_TEST_MESSAGE("Property name ="<<p->getName());
    for (uint32_t i=0 ; i < p->size() ; i++ ) {
        switch (p->getType()) {
        case xpcf::IProperty::IProperty_NONE :
            break;
        case xpcf::IProperty::IProperty_INTEGER :
            BOOST_TEST_MESSAGE("=> Int property = "<<p->getIntegerValue(i));
            break;
        case xpcf::IProperty::IProperty_UINTEGER :
            BOOST_TEST_MESSAGE("=> Uint property = "<<p->getUnsignedIntegerValue(i));
            break;
        case xpcf::IProperty::IProperty_LONG :
            BOOST_TEST_MESSAGE("=> Long property = "<<p->getLongValue(i));
            break;
        case xpcf::IProperty::IProperty_ULONG :
            BOOST_TEST_MESSAGE("=> ULong property = "<<p->getUnsignedLongValue(i));
            break;
        case xpcf::IProperty::IProperty_CHARSTR :
            BOOST_TEST_MESSAGE("=> String property = "<<p->getStringValue(i));
            break;
        case xpcf::IProperty::IProperty_UNICODESTR :
            break;
        case xpcf::IProperty::IProperty_FLOAT :
            BOOST_TEST_MESSAGE("=> Float property = "<<p->getFloatingValue(i));
            break;
        case xpcf::IProperty::IProperty_DOUBLE :
            BOOST_TEST_MESSAGE("=> Double property = "<<p->getDoubleValue(i));
            break;
        case xpcf::IProperty::IProperty_STRUCTURE :
        {
            SRef<xpcf::IPropertyMap> propertyMap = p->getStructureValue(i);
            BOOST_TEST_MESSAGE("Accessing class values for C0 from IProperty/IPropertyMap interfaces");
            for (auto property : propertyMap->getProperties()) {
                displayParameter(property);
            }
            break;
        }
        default:
            break;
        }
    }
}


BOOST_FIXTURE_TEST_CASE( test_component_creation,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    SRef<xpcf::IComponentIntrospect> rIComponentIntrospect;

    SRef<xpcf::IConfigurable> rIConfigurable;

    SRef<I2> rI2;
    xpcfComponentManager->load() ;
    // create a component
    //
    try {
        BOOST_REQUIRE_NO_THROW( rIConfigurable = xpcfComponentManager->create<component::C1>()->bindTo<xpcf::IConfigurable>() );
        BOOST_CHECK_THROW( rIComponentIntrospect = xpcfComponentManager->create<fakeComponent>(),xpcf::ComponentNotFoundException);
        BOOST_CHECK_THROW( rIConfigurable->bindTo<I0>(),xpcf::InterfaceNotImplementedException);
        BOOST_REQUIRE_NO_THROW(rIComponentIntrospect = xpcfComponentManager->create<component::C0>() );
        BOOST_TEST_REQUIRE(! rIComponentIntrospect->implements<xpcf::IConfigurable>(),"ERROR: C0 implements IConfigurable");
        SRef<I0> rI0 = rIComponentIntrospect->queryInterface<I0>(iid_I0);
        BOOST_TEST_REQUIRE(rI0, "ERROR:queryInterface(iid_I0,rI0)");
        BOOST_TEST_MESSAGE("TC1 OK. CreateInstance(clsid_C0, iid_I0 ,rI0)");
        //rIConfigurable = rIComponentIntrospect->queryInterface<xpcf::IConfigurable>();
        //rIComponentIntrospect->queryInterface(xpcf::toUUID<xpcf::IConfigurable>(),rIConfigurable);

        SRef<xpcf::IProperty> varSetted = rIConfigurable->getProperty("blurFactor");
        varSetted->setFloatingValue(4.0);

        BOOST_TEST_MESSAGE("Accessing class values for C0 from IProperty/IPropertyMap interfaces");
        for (auto property : rIConfigurable->getProperties()) {
            displayParameter(property);
        }
    }
    catch (std::out_of_range & e) {
        BOOST_TEST_MESSAGE("Catched : "<<e.what());
    }
}

BOOST_FIXTURE_TEST_CASE( test_component_introspection,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    SRef<xpcf::IComponentIntrospect> rIComponentIntrospect;


    xpcfComponentManager->load() ;
    /**
    *** 1) from a reference of the implementation class I0 (SRef<I0>) we obtain
    ***    the number of interfaces implemnted by the component class is(I0) belonging to.
    *** 2) from a reference of the implementation class I0 (SRef<I0>) we fill an
    ***    array (of xpcf::uuids::uuid) up interfaces uuids.
    *** 3) with the interface UUIDs we enumerate each interface help string.
    **/
    rIComponentIntrospect=   xpcfComponentManager->createComponent(clsid_C0);
    SRef<I0> rI0 = rIComponentIntrospect->queryInterface<I0>(iid_I0);

    uint32_t nbI = rI0->getNbInterfaces();
    BOOST_TEST_MESSAGE("TC7-1 OK. "<<nbI<<" Interface(s) found from I0:");

    BOOST_TEST_MESSAGE("TC7-2 OK. Enumerating uuids");
    unsigned long index = 0;

    for (xpcf::uuids::uuid interfaceUUID : rI0->getInterfaces()) {
        BOOST_TEST_MESSAGE("TC7-3-["<<index<<"] OK. Interface: "<<xpcf::uuids::to_string(interfaceUUID).c_str());
        index++;
    }
}

/*------------------------------*/
BOOST_FIXTURE_TEST_CASE( test_component_invocation,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    SRef<xpcf::IComponentIntrospect> rIComponentIntrospect;

    xpcfComponentManager->load() ;

    rIComponentIntrospect=   xpcfComponentManager->createComponent(clsid_C0);
    SRef<I0> rI0 = rIComponentIntrospect->queryInterface<I0>(iid_I0);
    BOOST_CHECK_THROW(SRef<I1> rI1test = rIComponentIntrospect->queryInterface<I1>(iid_I0),xpcf::utils::bad_any_cast);

    BOOST_TEST_MESSAGE("TC8-1 OK Calling methods of I0 on component C0");
    rI0->I0_opname();
    rI0->I0_opname2();
    rI0->I0_opname4();

    // look for another interface
    //
    //rI0->queryInterface(iid_I1, rI1);
    SRef<I1> rI1 = rIComponentIntrospect->queryInterface<I1>(iid_I1);
    BOOST_TEST_MESSAGE("TC8-2 OK queryInterface<I1>(iid_I1);");
    // test it
    //
    BOOST_TEST_MESSAGE("TC8-3 OK Calling methods of I1 on component C0");
    rI1->I1_opname();
    rI1->I1_opname2();
    rI1->I1_opname3();
    rI1->I1_opname4();

    rI1 = xpcfComponentManager->create<component::C1>()->bindTo<I1>();
    BOOST_TEST_MESSAGE("TC8-4 OK Calling methods of I1 on component C1");
    rI1->I1_opname();
    rI1->I1_opname2();
    rI1->I1_opname3();
    rI1->I1_opname4();

    SRef<I2>  rI2 = rI1->bindTo<I2>();
    BOOST_TEST_MESSAGE("TC8-5 O5 Calling methods of I1 on component C1 through I2 SRef");
    rI2->I1_opname();
    rI2->I1_opname2();
    rI2->I1_opname3();
    rI2->I1_opname4();

    BOOST_TEST_MESSAGE("TC8-6 OK Calling methods of I2 on component C1");
    rI2->I2_opname();
    rI2->I2_opname2();

}

BOOST_AUTO_TEST_SUITE_END()


