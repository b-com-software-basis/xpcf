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
 * @date 2017-04-28
 */


#include "ComponentManager.h"
#include "ContainerFactory.h"
#include <iostream>
#include <fstream>
#include <boost/dll.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/function.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "tinyxmlhelper.h"



using namespace std;

//namespace logging = boost::log;

namespace fs = boost::filesystem;

namespace org {
namespace bcom {
namespace xpcf {

std::atomic<ComponentManager*> ComponentManager::m_instance;
std::mutex ComponentManager::m_mutex;

ComponentManager * ComponentManager::instance()
{

    ComponentManager* compMgrInstance= m_instance.load(std::memory_order_acquire);
    if ( !compMgrInstance ){
        std::lock_guard<std::mutex> myLock(m_mutex);
        compMgrInstance = m_instance.load(std::memory_order_relaxed);
        if( !compMgrInstance ){
            compMgrInstance= new ComponentManager();
            m_instance.store(compMgrInstance,std::memory_order_release);
        }
    }
    return compMgrInstance;
}


SRef<IComponentManager> getComponentManagerInstance() {
    SRef<xpcf::IComponentIntrospect> lrIComponentIntrospect;
    SRef<IComponentManager> lrIComponentManager;
    xpcf::ComponentFactory::createComponent<ComponentManager>(lrIComponentIntrospect);
    lrIComponentIntrospect->queryInterface(toUUID(IComponentManager::UUID),lrIComponentManager);
    return lrIComponentManager;
}

template<> ComponentManager* org::bcom::xpcf::ComponentFactory::createInstance() {
    return ComponentManager::instance();
}

ComponentManager::ComponentManager():m_libraryLoaded(false)
{
    uuids::uuid cidComponentManager = org::bcom::xpcf::toUUID(ComponentManager::UUID );
    setUUID(cidComponentManager);
    addInterface<IComponentManager>(this,IComponentManager::UUID, "interface IComponentManager");
    //  m_logger.add_attribute("ClassName", boost::log::attributes::constant<std::string>("ComponentManager"));
    //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Constructor ComponentManager::ComponentManager () called!";
}

void ComponentManager::unloadComponent ()
{
    //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"ComponentManager::unload () called!";
}

void ComponentManager::addContainerRef(const uuids::uuid& containerUUID)
{

}

void ComponentManager::releaseContainerRef(const uuids::uuid& containerUUID)
{

}


unsigned long ComponentManager::load()
{
    return load(getenv( "XPCF_REGISTRY_PATH" ));
}

unsigned long ComponentManager::load(const char* libraryFilePath)
{
    unsigned long result = XPCF_OK;

    fs::path libraryConfigFilePath = libraryFilePath;
    std::ifstream libraryConfig(libraryConfigFilePath.string());
    if (libraryConfig.good()) {
        result = loadLibrary(libraryConfigFilePath);
    }
    else result = XPCF_FAIL;
    return result;
}

unsigned long ComponentManager::createComponent(const uuids::uuid& componentUUID, SRef<IComponentIntrospect>& componentRef)
{
    unsigned long errCode = XPCF_OK;

    if (isLoaded()) {
        SPtr<ComponentMetadata> componentInfos = findComponentMetadata(componentUUID);
        if (!componentInfos) {
            return XPCF_NOGETCOMPONENT;
        }
        SPtr<ContainerMetadata> containerInfos;
        if (m_containerMap.find(componentInfos->getContainerUUID()) != m_containerMap.end()) {
            containerInfos = m_containerMap.at(componentInfos->getContainerUUID());
        }
        if (!containerInfos) {
            return XPCF_NOGETCOMPONENT;
        }

        boost::dll::shared_library shlib;
        shlib.load(containerInfos->getPath());
        if (shlib.has(XPCF_GETCOMPONENT)) {
            if (m_funcMap.find(containerInfos->getUUID()) == m_funcMap.end()) {
                m_funcMap[containerInfos->getUUID()]=boost::dll::import<long(const uuids::uuid &, SRef<IComponentIntrospect>&)>(
                            containerInfos->getPath(), XPCF_GETCOMPONENT
                            );
            }

            if (m_funcMap.at(containerInfos->getUUID())) {
                m_funcMap.at(containerInfos->getUUID())(componentUUID,componentRef);
                if (componentRef) {
                    addContainerRef(containerInfos->getUUID());
                }
            }

        }
        else {
            errCode = XPCF_NOGETCOMPONENT;
        }
    }
    return errCode;

}

SPtr<ContainerMetadata> ComponentManager::introspectContainer(const char* containerFilePath)
{
    SPtr<ContainerMetadata> containerInfos;

    boost::function<const char* (void)> getContainerUUID = boost::dll::import<const char * (void)>(
                containerFilePath, XPCF_GETCONTAINERUUID
                );
    boost::function<const char* (void)> getContainerName = boost::dll::import<const char * (void)>(
                containerFilePath, XPCF_GETCONTAINERNAME
                );
    if (getContainerUUID && getContainerName) {
        std::string containerUUID = getContainerUUID();
        std::string containerName = getContainerName();
        if (m_containerMap.find(toUUID(containerUUID.c_str())) == m_containerMap.end()) {
            containerInfos = utils::make_shared<ContainerMetadata>(containerName.c_str(),containerUUID.c_str(),containerFilePath);
            getContainerComponentList(containerFilePath,containerInfos);
        }
        else { //Container has already been loaded in the library
            containerInfos = m_containerMap.at(toUUID(containerUUID.c_str()));
        }
    }
    return containerInfos;
}

SPtr<ContainerMetadata> ComponentManager::getContainerComponentList(const uuids::uuid& componentUUID)
{
    SPtr<ContainerMetadata> containerInfos;

    if (isLoaded()) {
        SPtr<ComponentMetadata> componentInfos = findComponentMetadata(componentUUID);
        if (componentInfos) {

            SPtr<ContainerMetadata> containerInfos;
            if (m_containerMap.find(componentInfos->getContainerUUID()) != m_containerMap.end()) {
                containerInfos = m_containerMap.at(componentInfos->getContainerUUID());
            }
        }
    }
    return containerInfos;
}

unsigned long ComponentManager::getContainerComponentList(const char *containerPath, SPtr<ContainerMetadata> containerInfos)
{
    unsigned long errCode = XPCF_OK;
    boost::function<unsigned long(void)> getNbComponents = boost::dll::import<unsigned long(void)>(
                containerPath, XPCF_GETNBCOMPONENTS
                );
    boost::function<const char* (unsigned long)> getComponentUUID = boost::dll::import<const char * (unsigned long)>(
                containerPath, XPCF_GETCOMPONENTUUID
                );
    if (getNbComponents && getComponentUUID) {
        for (unsigned long i = 0 ; i<getNbComponents() ; i++) {
            containerInfos->addComponent(toUUID(getComponentUUID(i)));
        }
    }
    else {
        errCode = XPCF_NOGETCOMPONENT;
    }

    return errCode;
}

unsigned long ComponentManager::addComponentMetadata(SPtr<ComponentMetadata> aClass)
{
    unsigned long result = XPCF_OK;

    if (aClass) {
        SPtr<ComponentMetadata> classExist = findComponentMetadata(aClass->getUUID());

        if (classExist) {
            result = XPCF_FAIL;
        }
        else {
            m_componentsVector.push_back(aClass);
        }
    }
    else {
        result = XPCF_FAIL;
    }

    return result;
}


unsigned long ComponentManager::addInterfaceMetadata(SPtr<InterfaceMetadata> aClass)
{
    unsigned long result = XPCF_OK;

    if (aClass) {
        SPtr<InterfaceMetadata> classExist = findInterfaceMetadata(aClass->getUUID());

        if (classExist) {
            result = XPCF_FAIL;
        }
        else {
            m_interfacesVector.push_back(aClass);
        }
    }
    else {
        result = XPCF_FAIL;
    }

    return result;
}


// Load the library from the file given
// by <aPath>
//
/********* The library file looks like this*************************************************
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<xpcf-library>
    <container uuid="E8CFAED2-1D86-4A5E-BDC9-389CFB9C38FA" filepath="xpcftest01.[so|dll|dylib]">
        <component uuid="097fee80-7bd4-47c2-9cc7-c1750e7beb2a" description="C0 component">
                TOADD: <constructor arg-0=UUIDOTHERCOMPONENT , arg-1=configoptions/>
                <interface uuid="87fb1573-defe-4c8f-8cba-304b888639cc" description="IComponentIntrospect"/>
                <interface uuid="c3eb5521-16ee-458e-9e77-157827753e46" description="I0 openCV scale interface"/>
                <interface uuid="e0b0a9af-a941-408e-aa10-cac5055af75a" description="I1 openCV rotate interface"/>
            </component>
    </container>
    <container uuid="7c8f9f3a-33f6-11d5-bc7f-00d0b7a62da9" filepath="xpcftest02.[so|dll|dylib]">
        <component uuid="cf0f569a-2f66-11d5-bc7f-00d0b7a62da9" description="xxx">
                <interface uuid="5f7c9b5e-ee9f-11d3-8010-000629ee2d57" description="IComponentIntrospect"/>
                <interface uuid="9db144b8-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
                <interface uuid="c5d2b99a-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
        </component>
    </container>
    <container uuid="b89994bc-3424-11d5-bc7f-00d0b7a62da9" filepath="xpcftest03.[so|dll|dylib]">
        <component uuid="cf0f569a-2f66-11d5-bc7f-00d0b7a62da9" description="xxx">
                <interface uuid="5f7c9b5e-ee9f-11d3-8010-000629ee2d57" description="IComponentIntrospect"/>
                <interface uuid="9db144b8-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
                <interface uuid="c5d2b99a-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
        </component>
        <component uuid="c2095c04-2f69-11d5-bc7f-00d0b7a62da9" description="xxx">
                <interface uuid="5f7c9b5e-ee9f-11d3-8010-000629ee2d57" description="IComponentIntrospect"/>
                <interface uuid="c5d2b99a-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
        </component>
    </container>
</xpcf-library>

*********************************************************************************************/
unsigned long ComponentManager::loadLibrary(fs::path containerPath)
{
    unsigned long result = XPCF_OK;
    SPtr<ContainerMetadata> containerInfo;
    SPtr<ComponentMetadata> componentInfo;
    SPtr<InterfaceMetadata>	  interfaceInfo;
    tinyxml2::XMLDocument doc;
    enum tinyxml2::XMLError loadOkay = doc.LoadFile(containerPath.string().c_str());
    if (loadOkay == 0) {
        try {
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Parsing XML from "<<containerPath<<" config file";
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"NOTE : Command line arguments are overloaded with config file parameters";
            //TODO : check each element exists before using it !
            tinyxml2::XMLElement * rootElt = doc.RootElement();
            tinyxml2::XMLElement * xmlContainerElt = rootElt->FirstChildElement("container");
            while (xmlContainerElt != NULL) {
                string filepath = xmlContainerElt->Attribute("filepath");
                string containerUuidStr = xmlContainerElt->Attribute("uuid");
                uuids::uuid containerUuid = toUUID(containerUuidStr.c_str());
                if (m_containerMap.find(containerUuid) == m_containerMap.end()) {
                    string containerDescription = "ContainerDesc";
                    containerInfo	= utils::make_shared<ContainerMetadata>(containerDescription.c_str(),containerUuid,filepath.c_str());
                    m_containerMap[containerUuid] = containerInfo;
                    tinyxml2::XMLElement *componentElt = xmlContainerElt->FirstChildElement("component");
                    while (componentElt != NULL) {
                        string componentUuidStr = componentElt->Attribute("uuid");
                        uuids::uuid componentUuid = toUUID(componentUuidStr.c_str());
                        if (m_componentsMap.find(componentUuid) == m_componentsMap.end()) {
                            string description = componentElt->Attribute("description");
                            componentInfo	= utils::make_shared<ComponentMetadata>(description.c_str(),componentUuid,containerUuid);
                            m_componentsVector.push_back(componentInfo);
                            m_componentsMap[componentUuid] = componentInfo;
                            containerInfo->addComponent(componentUuid);
                            tinyxml2::XMLElement *interfaceElt = componentElt->FirstChildElement("interface");
                            while (interfaceElt != NULL) {
                                //TODO : check the same IF doesn't appear twice in the vector !! ???
                                string interfaceUuidStr = interfaceElt->Attribute("uuid");
                                uuids::uuid interfaceUuid = toUUID(interfaceUuidStr.c_str());
                                componentInfo->addInterface(interfaceUuid);
                                if (m_interfacesMap.find(interfaceUuid) == m_interfacesMap.end()) {
                                    string interfaceDescription = interfaceElt->Attribute("description");
                                    interfaceInfo	= utils::make_shared<InterfaceMetadata>(interfaceDescription.c_str(),interfaceUuid);
                                    m_interfacesVector.push_back(interfaceInfo);
                                    m_interfacesMap[interfaceUuid]=interfaceInfo;
                                }
                                interfaceElt = interfaceElt->NextSiblingElement("interface");
                            }
                        }
                        componentElt = componentElt->NextSiblingElement("component");
                    }
                }
                xmlContainerElt = xmlContainerElt->NextSiblingElement();
            }
        }
        catch (const std::runtime_error & e) {
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"XML parsing file "<<containerPath<<" failed with error : "<<e.what();
            return -1;
        }
    }
    m_libraryLoaded = true;
    return result;
}

// TODO : study which method signature ? with container ID ? or ContainerMetadata ?
// What about default configuration for the component ?
unsigned long ComponentManager::saveContainerInformations(const SPtr<ContainerMetadata> & containerInfos)
{
    fs::path containerPath = containerInfos->getPath();
    unsigned long result = XPCF_OK;
    SPtr<ContainerMetadata> containerInfo;
    SPtr<ComponentMetadata> componentInfo;
    SPtr<InterfaceMetadata>	  interfaceInfo;
    tinyxml2::XMLDocument doc;
    enum tinyxml2::XMLError loadOkay = doc.LoadFile(containerPath.string().c_str());
    if (loadOkay == 0) {
        try {
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Parsing XML from "<<containerPath<<" config file";
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"NOTE : Command line arguments are overloaded with config file parameters";
            //TODO : check each element exists before using it !
            tinyxml2::XMLElement * rootElt = doc.RootElement();
            tinyxml2::XMLElement * xmlContainerElt = rootElt->FirstChildElement("container");
            while (xmlContainerElt != NULL) {
                string filepath = xmlContainerElt->Attribute("filepath");
                string containerUuidStr = xmlContainerElt->Attribute("uuid");
                uuids::uuid containerUuid = toUUID(containerUuidStr.c_str());
                string containerDescription = "ContainerDesc";
                containerInfo	= utils::make_shared<ContainerMetadata>(containerDescription.c_str(),containerUuid,filepath.c_str());
                m_containerMap[containerUuid] = containerInfo;
                tinyxml2::XMLElement *componentElt = xmlContainerElt->FirstChildElement("component");
                while (componentElt != NULL) {
                    string componentUuidStr = componentElt->Attribute("uuid");
                    uuids::uuid componentUuid = toUUID(componentUuidStr.c_str());
                    string componentDescription = componentElt->Attribute("description");
                    componentInfo	= utils::make_shared<ComponentMetadata>(componentDescription.c_str(),componentUuid,containerUuid);
                    m_componentsVector.push_back(componentInfo);
                    m_componentsMap[componentUuid] = componentInfo;
                    containerInfo->addComponent(componentUuid);
                    tinyxml2::XMLElement *interfaceElt = componentElt->FirstChildElement("interface");
                    while (interfaceElt != NULL) {
                        //TODO : check the same IF doesn't appear twice in the vector !! ???
                        string interfaceUuidStr = interfaceElt->Attribute("uuid");
                        uuids::uuid interfaceUuid = toUUID(interfaceUuidStr.c_str());
                        string interfaceDescription = interfaceElt->Attribute("description");
                        componentInfo->addInterface(interfaceUuid);
                        interfaceInfo	= utils::make_shared<InterfaceMetadata>(interfaceDescription.c_str(),interfaceUuid);
                        m_interfacesMap[interfaceUuid] = interfaceInfo;
                        m_interfacesVector.push_back(interfaceInfo);
                        interfaceElt = interfaceElt->NextSiblingElement("interface");
                    }
                    componentElt = componentElt->NextSiblingElement("component");
                }
                xmlContainerElt = xmlContainerElt->NextSiblingElement();
            }
        }
        catch (const std::runtime_error & e) {
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"XML parsing file "<<containerPath<<" failed with error : "<<e.what();
            return -1;
        }
    }
    m_libraryLoaded = true;
    return result;
}

// Given a UUID, we return a ComponentMetadata
// if no component is found with the UUID <clsid>
// a NULL smart pointer is returned.
//
SPtr<ComponentMetadata> ComponentManager::findComponentMetadata(const uuids::uuid& componentUUID) const
{
    SPtr<ComponentMetadata> result;
    if (m_componentsMap.find(componentUUID) != m_componentsMap.end()) {
        result = m_componentsMap.at(componentUUID);
    }
    return result;
}

SPtr<InterfaceMetadata> ComponentManager::findInterfaceMetadata(const uuids::uuid& interfaceUUID) const
{
    SPtr<InterfaceMetadata> result;
    if (m_interfacesMap.find(interfaceUUID) != m_interfacesMap.end()) {
        result = m_interfacesMap.at(interfaceUUID);
    }
    return result;
}

bool ComponentManager::isLoaded() const
{
    return m_libraryLoaded;
}

int ComponentManager::getNbComponentMetadatas() const
{
    return m_componentsVector.size();
}

SPtr<ComponentMetadata> ComponentManager::getComponentMetadata(int numComponent) const
{
    return m_componentsVector[numComponent];
}

int ComponentManager::getNbInterfaceMetadatas() const
{
    return m_interfacesVector.size();
}

SPtr<InterfaceMetadata> ComponentManager::getInterfaceMetadata(int i) const
{
    return m_interfacesVector[i];
}

}}} //namespace org::bcom::xpcf
