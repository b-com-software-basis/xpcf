/**
 * @copyright Copyright (c) 2019 B-com http://www.b-com.com/
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
 * @date 2019-06-05
 */

#include "xpcf/api/IConfigurable.h"
#include "ModuleManager.h"
#include "Registry.h"
#include "xpcf/component/ComponentFactory.h"
#include "xpcf/core/Exception.h"
#include "xpcf/core/helpers.h"
#include "PathBuilder.h"



#include <iostream>
#include <fstream>

using namespace std;
using std::placeholders::_1;

//namespace logging = boost::log;

#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
namespace fs = boost::filesystem;


XPCF_DEFINE_FACTORY_CREATE_INSTANCE(org::bcom::xpcf::Registry);

namespace org { namespace bcom { namespace xpcf {

Registry::Registry():ComponentBase (toUUID<Registry>())
{
    declareInterface<IRegistry>(this);
    declareInjectable<IAliasManager>(m_aliasManager);
}

uuids::uuid Registry::getModuleUUID(const uuids::uuid & componentUUID) const
{
    if (! mapContains(m_componentModuleUUIDMap,componentUUID)) {
        throw ModuleNotFoundException("No module UUID found for component "+uuids::to_string(componentUUID));
    }
    return m_componentModuleUUIDMap.at(componentUUID);
}

void Registry::clear()
{
    m_modulesVector.clear();
    m_interfacesVector.clear();
    m_componentModuleUUIDMap.clear();
    m_interfacesMap.clear();
    m_modulesMap.clear();
    m_libraryLoaded = false;
}

template <class T>
SPtr<T> findMetadata(const uuids::uuid & elementUUID, const std::map<uuids::uuid, SPtr<T>> & elementMap)
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

bool isXpcfFrameworkInterface(const uuids::uuid & interfaceUUID)
{
    return (interfaceUUID == toUUID<IComponentIntrospect>()) ||  (interfaceUUID == toUUID<IConfigurable>()) ||  (interfaceUUID == toUUID<IInjectable>());
}

// Given a UUID, we return a ComponentMetadata
// if no component is found with the UUID <clsid>
// a NULL smart pointer is returned.
//

SPtr<ModuleMetadata> Registry::findModuleMetadata(const uuids::uuid & moduleUUID) const
{
    return safeFindMetadata(moduleUUID, m_modulesMap);
}

SPtr<ComponentMetadata> Registry::findComponentMetadata(const uuids::uuid & componentUUID) const
{
    SPtr<ModuleMetadata> moduleInfo = findModuleMetadata(getModuleUUID(componentUUID));
    return moduleInfo->getComponentMetadata(componentUUID);
}

SPtr<InterfaceMetadata> Registry::findInterfaceMetadata(const uuids::uuid& interfaceUUID) const
{
    return findMetadata(interfaceUUID, m_interfacesMap);
}

// add metadata if not already present, otherwise does nothing
template <class T>
void addMetadata(SPtr<T> metadata, Collection<SPtr<T>,std::vector> & targetContainer, std::map<uuids::uuid, SPtr<T>> & elementMap)
{
    if (! metadata) {
        throw NullPointerException();
    }

    SPtr<T> classExist = findMetadata<T>(metadata->getUUID(), elementMap);
    if (!classExist) {
        targetContainer.add(metadata);
        elementMap[metadata->getUUID()] = metadata;
    }
}

void Registry::addModuleMetadata(SPtr<ModuleMetadata> metadata)
{
    addMetadata<ModuleMetadata>(metadata, m_modulesVector, m_modulesMap);
}

void Registry::addInterfaceMetadata(SPtr<InterfaceMetadata> metadata)
{
    addMetadata<InterfaceMetadata>(metadata, m_interfacesVector, m_interfacesMap);
}

void Registry::declareModuleMetadata(SPtr<ModuleMetadata> moduleInfos)
{
    addModuleMetadata(moduleInfos);
    for (SRef<ComponentMetadata> componentInfo : moduleInfos->getComponentsMetadata()) {
        if (! mapContains(m_componentModuleUUIDMap, componentInfo->getUUID()) ) {
            m_componentModuleUUIDMap[componentInfo->getUUID()] = moduleInfos->getUUID();
        }
        if (m_autoAlias) {
             if (!m_aliasManager->aliasExists(IAliasManager::Type::Component, componentInfo->name())) {
                m_aliasManager->declareAlias(IAliasManager::Type::Component, componentInfo->name(), componentInfo->getUUID());
             }
        }
        std::vector<SRef<InterfaceMetadata>> interfaceVector;
        SRef<xpcf::IComponentIntrospect> rIntrospect = getModuleManagerInstance()->createComponent(moduleInfos, componentInfo->getUUID());
        for (uuids::uuid interfaceUUID : rIntrospect->getInterfaces()) {
            autobind(interfaceUUID,componentInfo->getUUID());
            SRef<InterfaceMetadata> interfaceInfo = utils::make_shared<InterfaceMetadata>(rIntrospect->getMetadata(interfaceUUID));
            if (m_autoAlias) {
                 if (!m_aliasManager->aliasExists(IAliasManager::Type::Interface, interfaceInfo->name())) {
                    m_aliasManager->declareAlias(IAliasManager::Type::Interface, interfaceInfo->name(), interfaceInfo->getUUID());
                 }
            }
            componentInfo->addInterface(interfaceInfo->getUUID());
            addInterfaceMetadata(interfaceInfo);
        }
    }
}


void Registry::declareInterfaceNode(SRef<ComponentMetadata>  componentInfo, tinyxml2::XMLElement *interfaceElt)
{
    SPtr<InterfaceMetadata> interfaceInfo;
    string interfaceUuidStr = interfaceElt->Attribute("uuid");
    uuids::uuid interfaceUuid = toUUID(interfaceUuidStr);
    componentInfo->addInterface(interfaceUuid);
    autobind(interfaceUuid,componentInfo->getUUID());
    if (! mapContains(m_interfacesMap,interfaceUuid)) {
        string interfaceName = interfaceElt->Attribute("name");
        string interfaceDescription = interfaceElt->Attribute("description");
        if (m_autoAlias) {
            if (!m_aliasManager->aliasExists(IAliasManager::Type::Interface, interfaceName)) {
                m_aliasManager->declareAlias(IAliasManager::Type::Interface, interfaceName, interfaceUuid);
            }
        }
        interfaceInfo = utils::make_shared<InterfaceMetadata>(interfaceName.c_str(), interfaceUuid, interfaceDescription.c_str());
        addInterfaceMetadata(interfaceInfo);
    }
}

void Registry::declareComponent(SRef<ModuleMetadata> moduleInfo, tinyxml2::XMLElement *componentElt)
{
    string componentUuidStr = componentElt->Attribute("uuid");
    uuids::uuid componentUuid = toUUID(componentUuidStr);
    if (! mapContains(m_componentModuleUUIDMap,componentUuid)) {
        m_componentModuleUUIDMap[componentUuid] = moduleInfo->getUUID();
        string name = componentElt->Attribute("name");
        string description = componentElt->Attribute("description");
        SPtr<ComponentMetadata> componentInfo = utils::make_shared<ComponentMetadata>(name.c_str(), componentUuid, description.c_str());
        if (m_autoAlias) {
             if (!m_aliasManager->aliasExists(IAliasManager::Type::Component, name)) {
                m_aliasManager->declareAlias(IAliasManager::Type::Component, name, componentUuid);
             }
        }
        moduleInfo->addComponent(componentInfo);
        tinyxml2::XMLElement *interfaceElt = componentElt->FirstChildElement("interface");
        while (interfaceElt != nullptr) {
            //TODO : check the same IF doesn't appear twice in the vector !! ???
            declareInterfaceNode(componentInfo, interfaceElt);
            interfaceElt = interfaceElt->NextSiblingElement("interface");
        }
    }
}

void Registry::declareModule(tinyxml2::XMLElement * xmlModuleElt)
{
    SPtr<ModuleMetadata> moduleInfo;
    std::string moduleName = xmlModuleElt->Attribute("name");
    std::string moduleDescription = "";
    if (xmlModuleElt->Attribute("description") != nullptr) {
        moduleDescription = xmlModuleElt->Attribute("description");
    }
    uuids::uuid moduleUuid = toUUID( xmlModuleElt->Attribute("uuid"));
    moduleInfo = utils::make_shared<ModuleMetadata>(moduleName.c_str(), moduleUuid, moduleDescription.c_str(), xmlModuleElt->Attribute("path"));
    fs::path filePath = moduleInfo->getFullPath();
    // TODO : check lib existenz with file decorations
    /* if ( !boost::filesystem::exists( filePath ) )
    {
        std::cout<<"Error : Module not found at : "<<filePath.c_str()<<std::endl;
        return XPCFErrorCode::_FAIL;
    }*/

    if (! mapContains(m_modulesMap, moduleUuid)) {
        addModuleMetadata(moduleInfo);
    }
    // Add components to module even if there was a previous module declaration
    // Some components can be added later from another configuration
    tinyxml2::XMLElement *componentElt = xmlModuleElt->FirstChildElement("component");
    while (componentElt != nullptr) {
        declareComponent(moduleInfo , componentElt);
        componentElt = componentElt->NextSiblingElement("component");
    }
}

const IEnumerable<SPtr<ModuleMetadata>> & Registry::getModulesMetadata() const
{
    return m_modulesVector;
}

const IEnumerable<SPtr<InterfaceMetadata>> & Registry::getInterfacesMetadata() const
{
    return m_interfacesVector;
}

}}} //namespace org::bcom::xpcf
