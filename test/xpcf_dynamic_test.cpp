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
#include <boost/dll.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/function.hpp>
#include <boost/system/system_error.hpp>
#include <iostream>
#include <xpcf/core/uuid.h>
#include "xpcf/threading/FiberFifos.h"
#include "xpcf/threading/SharedBuffer.h"
#include "xpcf/threading/BaseTask.h"

#define BOOST_TEST_MODULE test dynamic xpcf
#include <boost/test/unit_test.hpp>

#include "TestDefines.h"

xpcf::uuids::string_generator gen;
xpcf::uuids::uuid clsid_HumanMusician = gen("ae9d10bb-4bd1-462b-8c01-43f1815c6ae0" );
xpcf::uuids::uuid iid_IComponentIntrospect= xpcf::toUUID<xpcf::IComponentIntrospect>();
xpcf::uuids::uuid iid_IHuman= xpcf::toUUID<IHuman>();
xpcf::uuids::uuid iid_IMusician= xpcf::toUUID<IMusician>();
xpcf::uuids::uuid iid_IGuitarist= xpcf::toUUID<IGuitarist>();

using namespace std;

//TODO : unittests, exception catching
/*------------------------------*/
void TCDLL()
{
    try {
        boost::dll::shared_library shlib;
        //shlib.load(boost::dll::program_location());
        fs::path p =  boost::dll::shared_library::decorate("/usr/lib/boost");
        BOOST_TEST_MESSAGE("Decorated path="<<p);
       /* shlib.load("test");
        if (shlib.has("my_test_func")) {
            boost::function<void(const char * )> testFunc = boost::dll::import<void(const char * )>(boost::dll::program_location(), "my_test_func");

            if (testFunc) {
                testFunc("message");
            }
        }*/

    }
    catch (boost::system::system_error & e) {
        BOOST_TEST_MESSAGE("Got exception");

    }
}



BOOST_AUTO_TEST_SUITE( test_library_component_metadata )

BOOST_AUTO_TEST_CASE( test_dll )
{
    TCDLL();
}

BOOST_AUTO_TEST_CASE( test_fixture )
{
    XpcfFixture fixture;
    SRef<xpcf::IComponentManager> localxpcfComponentManager;
    BOOST_TEST(fixture.xpcfComponentManager );
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

    pI=xpcfComponentManager->findInterfaceMetadata(iid_IHuman);

    BOOST_TEST_REQUIRE(pI,"ERROR Cannot find interface iid_IHuman !");
    BOOST_TEST_MESSAGE("Interface iid_IHuman found! its Description is:"<<pI->description());
}

BOOST_FIXTURE_TEST_CASE( test_read_components,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    xpcfComponentManager->load();
    /*
    ** 1)(using XPCF_Registry) getting number of component classes registrated
    ** 2)(using XPCF_Registry) getting a the component index=0 to obtain its (SPtr_XPCF_ComponentClass)
    ** 3)(using SPtr_XPCF_ComponentClass) obtaining the component class name (char*)
    ** 4)(using XPCF_Registry) finding a component classs from its UUID (clsid_HumanMusician) to get its (SPtr_XPCF_ComponentClass)
    ** 5)(using XPCF_Registry) finding a component classs from its name( help string) UUID (clsid_HumanMusician) to obtain its (xpcf::uuids::uuid)
    */
    BOOST_TEST_MESSAGE("Modules in registry : "<<xpcfComponentManager->getModulesMetadata().size()<<" module(s) found in registry");
    size_t nbComponents = 0;
    for (SPtr<xpcf::ModuleMetadata> moduleInfo : xpcfComponentManager->getModulesMetadata()) {
        nbComponents += moduleInfo->getComponentsMetadata().size();
    }
    BOOST_TEST_MESSAGE("Components in registry : "<<nbComponents<<" class(es) found in registry");

    SPtr<xpcf::ComponentMetadata> cmpMetadata;

    cmpMetadata= xpcfComponentManager->findComponentMetadata(clsid_HumanMusician);
    BOOST_TEST_REQUIRE(cmpMetadata,"ERROR Cannot get ComponentMetadata for component ID "<<clsid_HumanMusician<<" !");
    BOOST_TEST_MESSAGE("Component Class clsid_HumanMusician found! Its Description is:"<<cmpMetadata->description());
}

BOOST_FIXTURE_TEST_CASE( test_read_component,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    xpcfComponentManager->load();
    /*
    ** 1)(using the SPtr_XPCF_ComponentClass of clsid_HumanMusician)
    **    getting the path name of the Module library of the component Class
    ** 2) (using the SPtr_XPCF_ComponentClass of clsid_HumanMusician)
    **	  getting number of interfaces implemented by the component class "clsid_HumanMusician"
    ** 3) (using the SPtr_XPCF_ComponentClass of clsid_HumanMusician)
    **     Obtaining then re-setting the component Class Name(help string)
    **
    **
    */

    SPtr<xpcf::ComponentMetadata> cmpMetadata;
    cmpMetadata= xpcfComponentManager->findComponentMetadata(clsid_HumanMusician);

    BOOST_TEST_MESSAGE("Module UUID: "<<xpcfComponentManager->getModuleUUID(clsid_HumanMusician));

    int nbI = cmpMetadata->getInterfaces().size();
    BOOST_TEST_MESSAGE(nbI<<" interfaces found in component Class");


    /*BOOST_TEST_MESSAGE("Interface "<<cmpMetadata->GetInterface(nbI-1)<<"found in component Class" );*/

}

BOOST_FIXTURE_TEST_CASE( test_read_interfaces,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    xpcfComponentManager->load();
    /*
    ** 1) using XPCF_Registry
    **    getting the number of implemented interfaces of any components declared in the SCP registry file.
    ** 2) Getting pointers (SPtr<XPCF_Interface>) on all implemented interfaces of any components declared in the XPCF registry file.
    **     Then with these pointers we  obtain the name (help string) of each interface.
    **/
//xpcf::uuids::uuid * leakedUUID = new xpcf::uuids::uuid();
    SPtr<xpcf::InterfaceMetadata> interfaceMdata;
    uint32_t nbI = xpcfComponentManager->getInterfacesMetadata().size();
    BOOST_TEST_MESSAGE(nbI<<" interfaces found the registry s");
    for(auto interfaceMdata : xpcfComponentManager->getInterfacesMetadata())
    {
        BOOST_TEST_MESSAGE(interfaceMdata->getUUID()<<" OK. Interface Description: "<<interfaceMdata->description());
    }

}
/*------------------------------*/
BOOST_AUTO_TEST_CASE( test_shared_exception)
{

    /** 1) Declare a SPtr_XPCF_Interface (interfaceMdata2) then try to use it before
 ***    having setting it to a valid value.
 *** 2) using the XPCF_Exception reference "E" we obtain the error message.
 ***
 **/

    SPtr<xpcf::InterfaceMetadata> interfaceMdata2;
    /*       try
    {
        //should never occur.
                    BOOST_TEST_MESSAGE("Interface Description: "<<interfaceMdata2->getDescription());

    }
    catch(XPCF_NullObject * E)//WARNING : exception created in which scope ?? should it be sharedrefed ?
    {
        BOOST_TEST_MESSAGE("Testing exception. -Exception handled!- interfaceMdata2=null ");
        BOOST_TEST_MESSAGE("E.GetErrorMessage():\t"<<E->GetErrorMessage());
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
            SRef<xpcf::IEnumerator<SRef<xpcf::IProperty>>> propertyEnum =
                    propertyMap->getProperties().getEnumerator();
            BOOST_TEST_MESSAGE("Accessing class values for VirtualGuitarist from IProperty/IPropertyMap interfaces");
            while (propertyEnum->moveNext()) {
                displayParameter(propertyEnum->current());
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

    SRef<xpcf::IConfigurable> rIConfigurable,rITestInstance;

    SRef<IGuitarist> rIGuitarist;
    xpcfComponentManager->load() ;
    // create a component
    //
try {
    BOOST_REQUIRE_NO_THROW( rIConfigurable = xpcfComponentManager->create<component::VirtualGuitarist>()->bindTo<xpcf::IConfigurable>() );
    BOOST_REQUIRE_NO_THROW( rITestInstance = xpcfComponentManager->create<component::VirtualGuitarist>("testInstance")->bindTo<xpcf::IConfigurable>() );

    BOOST_CHECK_THROW( rIComponentIntrospect = xpcfComponentManager->create<fakeComponent>(),xpcf::ModuleNotFoundException);
    BOOST_CHECK_THROW( rIConfigurable->bindTo<IHuman>(),xpcf::InterfaceNotImplementedException);
    BOOST_REQUIRE_NO_THROW(rIComponentIntrospect = xpcfComponentManager->create<component::HumanMusician>() );
    BOOST_TEST_REQUIRE(! rIComponentIntrospect->implements<xpcf::IConfigurable>(),"ERROR: HumanMusician implements IConfigurable");
    SRef<IHuman> rIHuman = rIComponentIntrospect->queryInterface<IHuman>(iid_IHuman);
    BOOST_TEST_REQUIRE(rIHuman, "ERROR:queryInterface(iid_IHuman,rIHuman)");
    BOOST_TEST_MESSAGE("CreateInstance(clsid_HumanMusician, iid_IHuman ,rIHuman)");
    //rIConfigurable = rIComponentIntrospect->queryInterface<xpcf::IConfigurable>();
    //rIComponentIntrospect->queryInterface(xpcf::toUUID<xpcf::IConfigurable>(),rIConfigurable);

    SRef<xpcf::IProperty> varSetted = rIConfigurable->getProperty("blurFactor");
    varSetted->setFloatingValue(4.0);

    SRef<xpcf::IEnumerator<SRef<xpcf::IProperty>>> propertyEnum = rIConfigurable->getPropertiesEnumerator();
    BOOST_TEST_MESSAGE("Accessing class values for VirtualGuitarist from IProperty/IPropertyMap interfaces");
    while (propertyEnum->moveNext()) {
         displayParameter(propertyEnum->current());
    }

    BOOST_TEST_MESSAGE("Accessing instance values for VirtualGuitarist testInstance from IProperty/IPropertyMap interfaces");
    for (auto property : rITestInstance->getProperties()) {
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
    *** 1) from a reference of the implementation class IHuman (SRef<IHuman>) we obtain
    ***    the number of interfaces implemnted by the component class is(IHuman) belonging to.
    *** 2) from a reference of the implementation class IHuman (SRef<IHuman>) we fill an
    ***    array (of xpcf::uuids::uuid) up interfaces uuids.
    *** 3) with the interface UUIDs we enumerate each interface help string.
    **/
    rIComponentIntrospect=   xpcfComponentManager->createComponent(clsid_HumanMusician);
    SRef<IHuman> rIHuman = rIComponentIntrospect->queryInterface<IHuman>(iid_IHuman);

    uint32_t nbI = rIHuman->getNbInterfaces();
    BOOST_TEST_MESSAGE(nbI<<" Interface(s) found from IHuman:");

    BOOST_TEST_MESSAGE("Enumerating uuids");
    unsigned long index = 0;

    for (xpcf::uuids::uuid interfaceUUID : rIHuman->getInterfaces()) {
        BOOST_TEST_MESSAGE("["<<index<<"] OK. Interface: "<<xpcf::uuids::to_string(interfaceUUID).c_str());
        index++;
    }
}

/*------------------------------*/
BOOST_FIXTURE_TEST_CASE( test_component_invocation,XpcfFixture,* boost::unit_test::depends_on("test_library_component_metadata/test_load_library"))
{
    SRef<xpcf::IComponentIntrospect> rIComponentIntrospect;

    xpcfComponentManager->load() ;

    rIComponentIntrospect=   xpcfComponentManager->createComponent(clsid_HumanMusician);
    SRef<IHuman> rIHuman = rIComponentIntrospect->queryInterface<IHuman>(iid_IHuman);
    BOOST_CHECK_THROW(SRef<IMusician> rIMusiciantest = rIComponentIntrospect->queryInterface<IMusician>(iid_IHuman),xpcf::utils::bad_any_cast);

    BOOST_TEST_MESSAGE("Calling methods of IHuman on component HumanMusician");
    rIHuman->play();
    rIHuman->eat();
    rIHuman->work();

    // look for another interface
    //
    //rIHuman->queryInterface(iid_IMusician, rIMusician);
    SRef<IMusician> rIMusician = rIComponentIntrospect->queryInterface<IMusician>(iid_IMusician);
    BOOST_TEST_MESSAGE("queryInterface<IMusician>(iid_IMusician);");
    // test it
    //
    BOOST_TEST_MESSAGE("OK Calling methods of IMusician on component HumanMusician");
    rIMusician->learn();
    rIMusician->playMusic();
    rIMusician->listen();
    rIMusician->practice();

    rIMusician = xpcfComponentManager->create<component::VirtualGuitarist>()->bindTo<IMusician>();
    BOOST_TEST_MESSAGE("OK Calling methods of IMusician on component VirtualGuitarist");
    rIMusician->learn();
    rIMusician->playMusic();
    rIMusician->listen();
    rIMusician->practice();

    SRef<IGuitarist>  rIGuitarist = rIMusician->bindTo<IGuitarist>();
    BOOST_TEST_MESSAGE("Calling methods of IMusician on component VirtualGuitarist through IGuitarist SRef");
    rIGuitarist->learn();
    rIGuitarist->playMusic();
    rIGuitarist->listen();
    rIGuitarist->practice();

    BOOST_TEST_MESSAGE("OK Calling methods of IGuitarist on component VirtualGuitarist");
    rIGuitarist->playSolo();
    rIGuitarist->playRhythm();

}

BOOST_AUTO_TEST_SUITE_END()


