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

#include "xpcf/api/IConfigurable.h"
#include "xpcf/api/IModuleManager.h"
#include "ComponentManager.h"
#include "xpcf/core/Exception.h"
#include "PathBuilder.h"

#include <iostream>
#include <fstream>

using namespace std;

//namespace logging = boost::log;

namespace fs = boost::filesystem;

namespace org { namespace bcom { namespace xpcf {

std::atomic<ComponentManager*> ComponentManager::m_instance;
std::mutex ComponentManager::m_mutex;

ComponentManager * ComponentManager::instance()
{

    ComponentManager* compMgrInstance= m_instance.load(std::memory_order_acquire);
    if ( !compMgrInstance ){
        std::lock_guard<std::mutex> myLock(m_mutex);
        compMgrInstance = m_instance.load(std::memory_order_relaxed);
        if ( !compMgrInstance ){
            compMgrInstance= new ComponentManager();
            m_instance.store(compMgrInstance, std::memory_order_release);
        }
    }
    return compMgrInstance;
}


SRef<IComponentManager> getComponentManagerInstance()
{
    SRef<xpcf::IComponentIntrospect> lrIComponentIntrospect = xpcf::ComponentFactory::create<ComponentManager>();
    return lrIComponentIntrospect->bindTo<IComponentManager>();
}

template<> ComponentManager* ComponentFactory::createInstance()
{
    return ComponentManager::instance();
}

ComponentManager::ComponentManager():ComponentBase(toUUID<ComponentManager>()),m_libraryLoaded(false)
{
    addInterface<IComponentManager>(this);
    //  m_logger.add_attribute("ClassName", boost::log::attributes::constant<std::string>("ComponentManager"));
    //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Constructor ComponentManager::ComponentManager () called!";
}

void ComponentManager::unloadComponent ()
{
    //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"ComponentManager::unload () called!";
}


XPCFErrorCode ComponentManager::load()
{
    const char * registryPath = getenv( "XPCF_REGISTRY_PATH" );
    if (registryPath == nullptr) {
        registryPath = PathBuilder::getXPCFHomePath();
    }
    return load(registryPath);
}

XPCFErrorCode ComponentManager::load(const char* libraryFilePath)
{
    if (libraryFilePath == nullptr) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }

    fs::path libraryConfigFilePath = PathBuilder::replaceRootEnvVars(libraryFilePath);
    return loadLibrary(libraryConfigFilePath);
}

template <class T> void ComponentManager::load(fs::path folderPath)
{
    static_assert(std::is_same<T,fs::directory_iterator>::value || std::is_same<T,fs::recursive_directory_iterator>::value,
                  "Type passed to ComponentManager::load is neither a directory_iterator nor a recursive_directory_iterator");
    for (fs::directory_entry& x : T(folderPath)) {
        if (is_regular_file(x.path())) {
            if (x.path().extension() == ".xml") {
                loadLibrary(x.path());
            }
        }
    }
}

XPCFErrorCode ComponentManager::load(const char* folderPathStr, bool bRecurse)
{
    if (folderPathStr == nullptr) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }

    fs::path folderPath = PathBuilder::replaceRootEnvVars(folderPathStr);

    if ( ! fs::exists(folderPath)) {
        return XPCFErrorCode::_FAIL;
    }

    if (fs::is_directory(folderPath)) {
        if (bRecurse) {
            load<fs::recursive_directory_iterator>(folderPath);
        }
        else {
            load<fs::directory_iterator>(folderPath);
        }
    }
    else {
        loadLibrary(folderPath);
    }

    if (!m_libraryLoaded) {
        return XPCFErrorCode::_FAIL;
    }
    return XPCFErrorCode::_SUCCESS;
}

SRef<IComponentIntrospect> ComponentManager::createComponent(const uuids::uuid& componentUUID)
{
    XPCFErrorCode errCode = XPCFErrorCode::_SUCCESS;

    SPtr<ComponentMetadata> componentInfos = findComponentMetadata(componentUUID);
    if (!componentInfos) {
        throw ComponentNotFoundException("No component declared for UUID "+uuids::to_string(componentUUID));
    }

    if (m_moduleMap.find(componentInfos->getModuleUUID()) == m_moduleMap.end()) {
        throw ModuleNotFoundException("No module declared for UUID "+uuids::to_string(componentInfos->getModuleUUID()));
    }

    SPtr<ModuleMetadata> moduleInfos = m_moduleMap.at(componentInfos->getModuleUUID());

    SRef<IComponentIntrospect> componentRef = getModuleManagerInstance()->createComponent(moduleInfos, componentUUID);
    if (componentRef->implements<IConfigurable>()) {
       SRef<IConfigurable> iconf = componentRef->bindTo<IConfigurable>();
        iconf->configure(componentInfos->getConfigPath());
    }
    return componentRef;
}


SRef<IComponentIntrospect> ComponentManager::createComponent(const char * instanceName, const uuids::uuid& componentUUID)
{
    throw NotImplementedException();
    return nullptr;
}

template <class T>
SPtr<T> findMetadata(const uuids::uuid& elementUUID, const std::map<uuids::uuid, SPtr<T>> & elementMap)
{
    SPtr<T> result;
    if (elementMap.find(elementUUID) != elementMap.end()) {
        result = elementMap.at(elementUUID);
    }
    return result;
}

template <class T>
XPCFErrorCode addMetadata(SPtr<T> metadata, std::vector<SPtr<T>> & targetContainer, const std::map<uuids::uuid, SPtr<T>> & elementMap)
{
    if (! metadata) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }

    SPtr<T> classExist = findMetadata<T>(metadata->getUUID(), elementMap);
    if (classExist) {
        return XPCFErrorCode::_FAIL;
    }

    targetContainer.push_back(metadata);

    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode ComponentManager::addComponentMetadata(SPtr<ComponentMetadata> metadata)
{
    return addMetadata<ComponentMetadata>(metadata, m_componentsVector, m_componentsMap);
}


XPCFErrorCode ComponentManager::addInterfaceMetadata(SPtr<InterfaceMetadata> metadata)
{
    return addMetadata<InterfaceMetadata>(metadata, m_interfacesVector, m_interfacesMap);
}

XPCFErrorCode ComponentManager::declareInterface(uuids::uuid componentUuid, tinyxml2::XMLElement *interfaceElt)
{
    SPtr<InterfaceMetadata> interfaceInfo;
    string interfaceUuidStr = interfaceElt->Attribute("uuid");
    uuids::uuid interfaceUuid = toUUID(interfaceUuidStr.c_str());
    m_componentsMap[componentUuid]->addInterface(interfaceUuid);
    if (m_interfacesMap.find(interfaceUuid) == m_interfacesMap.end()) {
        string interfaceDescription = interfaceElt->Attribute("description");
        interfaceInfo = utils::make_shared<InterfaceMetadata>(interfaceDescription.c_str(), interfaceUuid);
        m_interfacesVector.push_back(interfaceInfo);
        m_interfacesMap[interfaceUuid]=interfaceInfo;
    }
    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode ComponentManager::declareComponent(uuids::uuid moduleUuid, tinyxml2::XMLElement *componentElt)
{
    SPtr<ComponentMetadata> componentInfo;
    string componentUuidStr = componentElt->Attribute("uuid");
    uuids::uuid componentUuid = toUUID(componentUuidStr.c_str());
    if (m_componentsMap.find(componentUuid) == m_componentsMap.end()) {
        string description = componentElt->Attribute("description");
        fs::path configPath = m_moduleConfigMap[moduleUuid];
        componentInfo = utils::make_shared<ComponentMetadata>(description.c_str(), componentUuid, moduleUuid, configPath.string().c_str());
        m_componentsVector.push_back(componentInfo);
        m_componentsMap[componentUuid] = componentInfo;
        m_moduleMap[moduleUuid]->addComponent(componentUuid);
        tinyxml2::XMLElement *interfaceElt = componentElt->FirstChildElement("interface");
        while (interfaceElt != NULL) {
            //TODO : check the same IF doesn't appear twice in the vector !! ???
            declareInterface(componentUuid, interfaceElt);
            interfaceElt = interfaceElt->NextSiblingElement("interface");
        }
    }
    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode ComponentManager::declareModule(tinyxml2::XMLElement * xmlModuleElt, fs::path modulePath)
{
    SPtr<ModuleMetadata> moduleInfo;
    std::string moduleName = xmlModuleElt->Attribute("name");
    uuids::uuid moduleUuid = toUUID( xmlModuleElt->Attribute("uuid"));
    moduleInfo = utils::make_shared<ModuleMetadata>(moduleName.c_str(), moduleUuid, xmlModuleElt->Attribute("path"));
    fs::path filePath = moduleInfo->getFullPath();
    // TODO : check lib existenz with file decorations
    /* if ( !boost::filesystem::exists( filePath ) )
    {
        std::cout<<"Error : Module not found at : "<<filePath.c_str()<<std::endl;
        return XPCFErrorCode::_FAIL;
    }*/

    if (m_moduleMap.find(moduleUuid) == m_moduleMap.end()) {
        m_moduleMap[moduleUuid] = moduleInfo;
        m_moduleConfigMap[moduleUuid] = modulePath;
        tinyxml2::XMLElement *componentElt = xmlModuleElt->FirstChildElement("component");
        while (componentElt != NULL) {
            declareComponent(moduleUuid, componentElt);
            componentElt = componentElt->NextSiblingElement("component");
        }
    }
    return XPCFErrorCode::_SUCCESS;
}

// Load the library from the file given
// by <modulePath>
//
/********* The library file looks like this*************************************************
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<xpcf-registry>
    <module uuid="E8CFAED2-1D86-4A5E-BDC9-389CFB9C38FA" name="xpcftest01" path="folderpath/">
        <component uuid="097fee80-7bd4-47c2-9cc7-c1750e7beb2a" description="C0 component">
                TOADD: <constructor arg-0=UUIDOTHERCOMPONENT , arg-1=configoptions/>
                <interface uuid="87fb1573-defe-4c8f-8cba-304b888639cc" description="IComponentIntrospect"/>
                <interface uuid="c3eb5521-16ee-458e-9e77-157827753e46" description="I0 openCV scale interface"/>
                <interface uuid="e0b0a9af-a941-408e-aa10-cac5055af75a" description="I1 openCV rotate interface"/>
            </component>
    </module>
    <module uuid="7c8f9f3a-33f6-11d5-bc7f-00d0b7a62da9" name="xpcftest02" path="folderpath2/">
        <component uuid="cf0f569a-2f66-11d5-bc7f-00d0b7a62da9" description="xxx">
                <interface uuid="5f7c9b5e-ee9f-11d3-8010-000629ee2d57" description="IComponentIntrospect"/>
                <interface uuid="9db144b8-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
                <interface uuid="c5d2b99a-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
        </component>
    </module>
    <module uuid="b89994bc-3424-11d5-bc7f-00d0b7a62da9" name="xpcftest03" path="folderpath3/">
        <component uuid="cf0f569a-2f66-11d5-bc7f-00d0b7a62da9" description="xxx">
                <interface uuid="5f7c9b5e-ee9f-11d3-8010-000629ee2d57" description="IComponentIntrospect"/>
                <interface uuid="9db144b8-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
                <interface uuid="c5d2b99a-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
        </component>
        <component uuid="c2095c04-2f69-11d5-bc7f-00d0b7a62da9" description="xxx">
                <interface uuid="5f7c9b5e-ee9f-11d3-8010-000629ee2d57" description="IComponentIntrospect"/>
                <interface uuid="c5d2b99a-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
        </component>
    </module>
</xpcf-library>

*********************************************************************************************/
XPCFErrorCode ComponentManager::loadLibrary(fs::path modulePath)
{
    if ( ! fs::exists(modulePath)) {
        return XPCFErrorCode::_FAIL;
    }
    if ( modulePath.extension() != ".xml") {
        return XPCFErrorCode::_FAIL;
    }

    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;
    tinyxml2::XMLDocument doc;
    enum tinyxml2::XMLError loadOkay = doc.LoadFile(modulePath.string().c_str());
    if (loadOkay == 0) {
        try {
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Parsing XML from "<<modulePath<<" config file";
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"NOTE : Command line arguments are overloaded with config file parameters";
            //TODO : check each element exists before using it !
            // a check should be performed upon announced module uuid and inner module uuid
            // check xml node is xpcf-registry first !
            tinyxml2::XMLElement * rootElt = doc.RootElement();
            string rootName = rootElt->Value();
            if (rootName != "xpcf-registry" && rootName != "xpcf-configuration") {
                return XPCFErrorCode::_ERROR_RANGE;
            }
            tinyxml2::XMLElement * xmlModuleElt = rootElt->FirstChildElement("module");
            while (xmlModuleElt != NULL && result == XPCFErrorCode::_SUCCESS) {
                result = declareModule(xmlModuleElt,modulePath);
                xmlModuleElt = xmlModuleElt->NextSiblingElement("module");
            }
        }
        catch (const std::runtime_error & e) {
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"XML parsing file "<<modulePath<<" failed with error : "<<e.what();
            return XPCFErrorCode::_FAIL;
        }
    }
    m_libraryLoaded = (result == XPCFErrorCode::_SUCCESS);
    return result;
}


// Given a UUID, we return a ComponentMetadata
// if no component is found with the UUID <clsid>
// a NULL smart pointer is returned.
//
SPtr<ComponentMetadata> ComponentManager::findComponentMetadata(const uuids::uuid& componentUUID) const
{
    return findMetadata(componentUUID, m_componentsMap);
}

SPtr<InterfaceMetadata> ComponentManager::findInterfaceMetadata(const uuids::uuid& interfaceUUID) const
{
    return findMetadata(interfaceUUID, m_interfacesMap);
}

uint32_t ComponentManager::getNbComponentMetadatas() const
{
    return m_componentsVector.size();
}

SPtr<ComponentMetadata> ComponentManager::getComponentMetadata(uint32_t numComponent) const
{
    return m_componentsVector[numComponent];
}

uint32_t ComponentManager::getNbInterfaceMetadatas() const
{
    return m_interfacesVector.size();
}

SPtr<InterfaceMetadata> ComponentManager::getInterfaceMetadata(uint32_t i) const
{
    return m_interfacesVector[i];
}

}}} //namespace org::bcom::xpcf
