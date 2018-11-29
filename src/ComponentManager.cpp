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
#include "ModuleManager.h"
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

uuids::uuid ComponentManager::getModuleUUID(uuids::uuid componentUUID) const
{
    if (m_componentModuleUUIDMap.find(componentUUID) == m_componentModuleUUIDMap.end()) {
        throw ModuleNotFoundException("No module UUID found for component "+uuids::to_string(componentUUID));
    }
    return m_componentModuleUUIDMap.at(componentUUID);
}

fs::path ComponentManager::getConfigPath(uuids::uuid componentUUID) const
{
    uuids::uuid moduleUUID = getModuleUUID(componentUUID);
    if (m_moduleConfigMap.find(moduleUUID) == m_moduleConfigMap.end()) {
        throw ModuleNotFoundException("No configuration path found for module "+uuids::to_string(moduleUUID));
    }
    return m_moduleConfigMap.at(moduleUUID);

}

void ComponentManager::releaseComponent(uuids::uuid componentUUID)
{
    //Note : release the module ref, or count moduleref per component uuid to check ??
    ModuleManager::instance()->releaseModuleRef(getModuleUUID(componentUUID));
}

void ComponentManager::unloadComponent ()
{
    //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"ComponentManager::unload () called!";
}

void ComponentManager::clear()
{
     m_modulesVector.clear();
     m_interfacesVector.clear();
     m_componentModuleUUIDMap.clear();
     m_interfacesMap.clear();
     m_modulesMap.clear();
     m_moduleConfigMap.clear();
     m_libraryLoaded = false;
}

XPCFErrorCode ComponentManager::load()
{
    const char * registryPath = getenv( "XPCF_REGISTRY_PATH" );
    if (registryPath != nullptr) {
        load(registryPath);
    }
    return load<fs::recursive_directory_iterator>(PathBuilder::getXPCFHomePath());
}

XPCFErrorCode ComponentManager::load(const char* libraryFilePath)
{
    if (libraryFilePath == nullptr) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }

    fs::path libraryConfigFilePath = PathBuilder::replaceRootEnvVars(libraryFilePath);
    return loadLibrary(libraryConfigFilePath);
}

template <class T> XPCFErrorCode ComponentManager::load(fs::path folderPath)
{
    //TODO : what strategy to report error of load for a dedicated file but load others ?
    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;
    static_assert(std::is_same<T,fs::directory_iterator>::value || std::is_same<T,fs::recursive_directory_iterator>::value,
                  "Type passed to ComponentManager::load is neither a directory_iterator nor a recursive_directory_iterator");
    for (fs::directory_entry& x : T(folderPath)) {
        if (is_regular_file(x.path())) {
            if (x.path().extension() == ".xml") {
                loadLibrary(x.path());
            }
        }
    }
    return result;
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

SRef<IComponentIntrospect> ComponentManager::create(const uuids::uuid& componentUUID)
{
    SPtr<ModuleMetadata> moduleInfos = findModuleMetadata(getModuleUUID(componentUUID));

    SRef<IComponentIntrospect> componentRef = getModuleManagerInstance()->createComponent(moduleInfos, componentUUID);
    return componentRef;
}

SRef<IComponentIntrospect> ComponentManager::createComponent(const uuids::uuid& componentUUID)
{
    SRef<IComponentIntrospect> componentRef = create(componentUUID);
    if (componentRef->implements<IConfigurable>()) {
        SRef<IConfigurable> iconf = componentRef->bindTo<IConfigurable>();
        iconf->configure(getConfigPath(componentUUID).string().c_str());
    }
    return componentRef;
}

SRef<IComponentIntrospect> ComponentManager::createComponent(const char * instanceName, const uuids::uuid& componentUUID)
{
    SRef<IComponentIntrospect> componentRef = create(componentUUID);
    if (componentRef->implements<IConfigurable>()) {
        SRef<IConfigurable> iconf = componentRef->bindTo<IConfigurable>();
        iconf->configure(getConfigPath(componentUUID).string().c_str(),instanceName);
    }
    return componentRef;
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


template<typename M>
struct inferException
{
};

template<>
struct inferException<ComponentMetadata>
{
    typedef ComponentNotFoundException InnerType;
};

template<>
struct inferException<ModuleMetadata>
{
    typedef ModuleNotFoundException InnerType;
};


template <typename T, typename E = typename inferException<T>::InnerType>
SPtr<T> safeFindMetadata(const uuids::uuid& elementUUID, const std::map<uuids::uuid, SPtr<T>> & elementMap)
{
    SPtr<T> result = findMetadata<T>(elementUUID, elementMap);
    if (!result) {
        throw E(elementUUID);
    }
    return result;
}

// Given a UUID, we return a ComponentMetadata
// if no component is found with the UUID <clsid>
// a NULL smart pointer is returned.
//

SPtr<ModuleMetadata> ComponentManager::findModuleMetadata(const uuids::uuid & moduleUUID) const
{
    return safeFindMetadata(moduleUUID, m_modulesMap);
}

SPtr<ComponentMetadata> ComponentManager::findComponentMetadata(const uuids::uuid & componentUUID) const
{
    SPtr<ModuleMetadata> moduleInfo = findModuleMetadata(getModuleUUID(componentUUID));
    return moduleInfo->getComponentMetadata(componentUUID);
}

SPtr<InterfaceMetadata> ComponentManager::findInterfaceMetadata(const uuids::uuid& interfaceUUID) const
{
    return findMetadata(interfaceUUID, m_interfacesMap);
}


template <class T>
XPCFErrorCode addMetadata(SPtr<T> metadata, Collection<SPtr<T>,std::vector> & targetContainer, std::map<uuids::uuid, SPtr<T>> & elementMap)
{
    if (! metadata) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }

    SPtr<T> classExist = findMetadata<T>(metadata->getUUID(), elementMap);
    if (classExist) {
        return XPCFErrorCode::_FAIL;
    }

    targetContainer.add(metadata);
    elementMap[metadata->getUUID()] = metadata;
    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode ComponentManager::addModuleMetadata(SPtr<ModuleMetadata> metadata)
{
    return addMetadata<ModuleMetadata>(metadata, m_modulesVector, m_modulesMap);
}


XPCFErrorCode ComponentManager::addInterfaceMetadata(SPtr<InterfaceMetadata> metadata)
{
    return addMetadata<InterfaceMetadata>(metadata, m_interfacesVector, m_interfacesMap);
}

XPCFErrorCode ComponentManager::declareInterface(SRef<ComponentMetadata>  componentInfo, tinyxml2::XMLElement *interfaceElt)
{
    SPtr<InterfaceMetadata> interfaceInfo;
    string interfaceUuidStr = interfaceElt->Attribute("uuid");
    uuids::uuid interfaceUuid = toUUID(interfaceUuidStr.c_str());
    componentInfo->addInterface(interfaceUuid);
    if (m_interfacesMap.find(interfaceUuid) == m_interfacesMap.end()) {
        string interfaceName = interfaceElt->Attribute("name");
        string interfaceDescription = interfaceElt->Attribute("description");
        interfaceInfo = utils::make_shared<InterfaceMetadata>(interfaceName.c_str(), interfaceUuid, interfaceDescription.c_str());
        addInterfaceMetadata(interfaceInfo);
    }
    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode ComponentManager::declareComponent(SRef<ModuleMetadata> moduleInfo, tinyxml2::XMLElement *componentElt)
{
    string componentUuidStr = componentElt->Attribute("uuid");
    uuids::uuid componentUuid = toUUID(componentUuidStr.c_str());
    if (m_componentModuleUUIDMap.find(componentUuid) == m_componentModuleUUIDMap.end()) {
        m_componentModuleUUIDMap[componentUuid] = moduleInfo->getUUID();
        string name = componentElt->Attribute("name");
        string description = componentElt->Attribute("description");
        SPtr<ComponentMetadata> componentInfo = utils::make_shared<ComponentMetadata>(name.c_str(), componentUuid, description.c_str());
        moduleInfo->addComponent(componentInfo);
        tinyxml2::XMLElement *interfaceElt = componentElt->FirstChildElement("interface");
        while (interfaceElt != NULL) {
            //TODO : check the same IF doesn't appear twice in the vector !! ???
            declareInterface(componentInfo, interfaceElt);
            interfaceElt = interfaceElt->NextSiblingElement("interface");
        }
    }
    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode ComponentManager::declareModule(tinyxml2::XMLElement * xmlModuleElt, fs::path configurationFilePath)
{
    SPtr<ModuleMetadata> moduleInfo;
    std::string moduleName = xmlModuleElt->Attribute("name");
    std::string moduleDescription = xmlModuleElt->Attribute("description");
    uuids::uuid moduleUuid = toUUID( xmlModuleElt->Attribute("uuid"));
    moduleInfo = utils::make_shared<ModuleMetadata>(moduleName.c_str(), moduleUuid, moduleDescription.c_str(), xmlModuleElt->Attribute("path"));
    fs::path filePath = moduleInfo->getFullPath();
    // TODO : check lib existenz with file decorations
    /* if ( !boost::filesystem::exists( filePath ) )
    {
        std::cout<<"Error : Module not found at : "<<filePath.c_str()<<std::endl;
        return XPCFErrorCode::_FAIL;
    }*/

    if (m_modulesMap.find(moduleUuid) == m_modulesMap.end()) {
        addModuleMetadata(moduleInfo);
        m_moduleConfigMap[moduleUuid] = configurationFilePath;

        tinyxml2::XMLElement *componentElt = xmlModuleElt->FirstChildElement("component");
        while (componentElt != NULL) {
            declareComponent(moduleInfo , componentElt);
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
XPCFErrorCode ComponentManager::loadLibrary(fs::path configurationFilePath)
{
    if ( ! fs::exists(configurationFilePath)) {
        return XPCFErrorCode::_FAIL;
    }
    if ( configurationFilePath.extension() != ".xml") {
        return XPCFErrorCode::_FAIL;
    }

    XPCFErrorCode result = XPCFErrorCode::_FAIL;
    tinyxml2::XMLDocument doc;
    enum tinyxml2::XMLError loadOkay = doc.LoadFile(configurationFilePath.string().c_str());
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
            result = XPCFErrorCode::_SUCCESS;
            tinyxml2::XMLElement * xmlModuleElt = rootElt->FirstChildElement("module");
            while (xmlModuleElt != NULL && result == XPCFErrorCode::_SUCCESS) {
                result = declareModule(xmlModuleElt,configurationFilePath);
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

const IEnumerable<SPtr<ModuleMetadata>> & ComponentManager::getModulesMetadata() const
{
    return m_modulesVector;
}

const IEnumerable<SPtr<InterfaceMetadata>> & ComponentManager::getInterfacesMetadata() const
{
    return m_interfacesVector;
}

}}} //namespace org::bcom::xpcf
