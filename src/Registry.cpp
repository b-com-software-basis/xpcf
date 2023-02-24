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
#include "private/xpcf/ModuleManager.h"
#include "private/xpcf/Registry.h"
#include "xpcf/component/ComponentFactory.h"
#include "xpcf/core/ErrorMessage.h"
#include "xpcf/core/Exception.h"
#include "xpcf/core/helpers.h"
#include "private/xpcf/PathBuilder.h"



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


void RegistryContext::clear()
{
    modulesVector.clear();
    interfacesVector.clear();
    componentModuleUUIDMap.clear();
    interfacesMap.clear();
    modulesMap.clear();
}

Registry::Registry():ComponentBase (toUUID<Registry>())
{
    m_context = utils::make_shared<RegistryContext>();
    declareInterface<IRegistryManager>(this);
    declareInterface<AbstractRegistry>(this);
    declareInjectable<IAliasManager>(m_aliasManager);
}

uuids::uuid Registry::getModuleUUID(const uuids::uuid & componentUUID) const
{
    if (! mapContains(m_context->componentModuleUUIDMap,componentUUID)) {
        throw ModuleNotFoundException("No module UUID found for component "+uuids::to_string(componentUUID));
    }
    return m_context->componentModuleUUIDMap.at(componentUUID);
}

void Registry::clear()
{
    m_context->clear();
    m_libraryLoaded = false;
}

XPCFErrorCode Registry::loadModuleMetadata(const char * moduleName,
                                          const char * moduleFilePath)
{
    SRef<ModuleMetadata> moduleInfos = getModuleManagerInstance()->introspectModule(moduleName, moduleFilePath);
    declareModuleMetadata(moduleInfos);
    return XPCFErrorCode::_SUCCESS;
}

template <class T> XPCFErrorCode Registry::loadModules(fs::path folderPath)
{
    //TODO : what strategy to report error of load for a dedicated file but load others ?
    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;
    static_assert(std::is_same<T,fs::directory_iterator>::value || std::is_same<T,fs::recursive_directory_iterator>::value,
            "Type passed to ComponentManager::load is neither a directory_iterator nor a recursive_directory_iterator");
    for (fs::directory_entry& x : T(folderPath)) {
        if (PathBuilder::is_shared_library(x.path())) {
            fs::path modulePath = x.path().parent_path();
            fs::path moduleName = x.path().filename();
            fs::detail::utf8_codecvt_facet utf8;
            result = loadModuleMetadata(moduleName.string(utf8).c_str(),modulePath.string(utf8).c_str());
        }
    }
    return result;
}

XPCFErrorCode Registry::loadModules(const char * folderPathStr, bool bRecurse)
{
    if (folderPathStr == nullptr) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }

    fs::path folderPath = PathBuilder::buildModuleFolderPath(folderPathStr);

    if ( ! fs::exists(folderPath)) {
        return XPCFErrorCode::_FAIL;
    }

    if ( !fs::is_directory(folderPath)) {
        return XPCFErrorCode::_FAIL;
    }
    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;

    if (bRecurse) {
        result = loadModules<fs::recursive_directory_iterator>(folderPath);
    }
    else {
        result = loadModules<fs::directory_iterator>(folderPath);
    }
    return result;
}

SRef<RegistryContext> Registry::getContext() const
{
    return m_context;
}

void Registry::setContext(SRef<RegistryContext> context)
{
    m_context = context;
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
    return safeFindMetadata(moduleUUID, m_context->modulesMap);
}

SPtr<ComponentMetadata> Registry::findComponentMetadata(const uuids::uuid & componentUUID) const
{
    SPtr<ModuleMetadata> moduleInfo = findModuleMetadata(getModuleUUID(componentUUID));
    return moduleInfo->getComponentMetadata(componentUUID);
}

SPtr<InterfaceMetadata> Registry::findInterfaceMetadata(const uuids::uuid& interfaceUUID) const
{
    return findMetadata(interfaceUUID, m_context->interfacesMap);
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
    addMetadata<ModuleMetadata>(metadata, m_context->modulesVector, m_context->modulesMap);
}

void Registry::addInterfaceMetadata(SPtr<InterfaceMetadata> metadata)
{
    addMetadata<InterfaceMetadata>(metadata, m_context->interfacesVector, m_context->interfacesMap);
}

void Registry::declareModuleMetadata(SPtr<ModuleMetadata> moduleInfos)
{
    addModuleMetadata(moduleInfos);
    for (SRef<ComponentMetadata> componentInfo : moduleInfos->getComponentsMetadata()) {
        if (! mapContains(m_context->componentModuleUUIDMap, componentInfo->getUUID()) ) {
            m_context->componentModuleUUIDMap[componentInfo->getUUID()] = moduleInfos->getUUID();
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
    string interfaceUuidStr = "";
    string interfaceName = "";
    try {
        SPtr<InterfaceMetadata> interfaceInfo;
        if(interfaceElt->Attribute("uuid") != nullptr) {
            interfaceUuidStr = interfaceElt->Attribute("uuid");
        }
        if(interfaceElt->Attribute("name") != nullptr) {
            interfaceName = interfaceElt->Attribute("name");
        }
        uuids::uuid interfaceUuid;
        try {
            interfaceUuid = toUUID(interfaceUuidStr);
        }
        catch(const std::runtime_error & e) {
            std::string what = "UUID format invalid in \"uuid\" attribute: ";
            what.append(interfaceElt->Attribute("uuid"));
            throw ConfigurationException(what);
        }
        componentInfo->addInterface(interfaceUuid);
        autobind(interfaceUuid,componentInfo->getUUID());
        if (! mapContains(m_context->interfacesMap,interfaceUuid)) {
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
    catch (const xpcf::Exception& e) {
        XmlErrorMessage errMsg("interface", e.what(), false);
        if(!interfaceUuidStr.empty()) {
            errMsg.addAttribute("uuid", interfaceUuidStr);
        }
        if(!interfaceName.empty()) {
            errMsg.addAttribute("name", interfaceName);
        }
        throw ConfigurationException(errMsg.write());
    }
}

void Registry::declareComponent(SRef<ModuleMetadata> moduleInfo, tinyxml2::XMLElement *componentElt)
{
    string componentUuidStr = "";
    string componentName = "";
    bool subElementError = false;
    try {
        if(componentElt->Attribute("uuid") != nullptr) {
            componentUuidStr = componentElt->Attribute("uuid");
        }
        if(componentElt->Attribute("name") != nullptr) {
            componentName = componentElt->Attribute("name");
        }
        uuids::uuid componentUuid;
        try {
            componentUuid = toUUID(componentUuidStr);
            subElementError = true;
        }
        catch(const std::runtime_error&) {
            std::string what = "UUID format invalid in \"uuid\": ";
            what.append(componentElt->Attribute("uuid"));
            throw ConfigurationException(what);
        }
        if (! mapContains(m_context->componentModuleUUIDMap,componentUuid)) {
            m_context->componentModuleUUIDMap[componentUuid] = moduleInfo->getUUID();
            string description = componentElt->Attribute("description");
            SPtr<ComponentMetadata> componentInfo = utils::make_shared<ComponentMetadata>(componentName.c_str(), componentUuid, description.c_str());
            if (m_autoAlias) {
                if (!m_aliasManager->aliasExists(IAliasManager::Type::Component, componentName)) {
                    m_aliasManager->declareAlias(IAliasManager::Type::Component, componentName, componentUuid);
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
    catch (const xpcf::Exception& e) {
        XmlErrorMessage errMsg("component", e.what(), subElementError);
        if(!componentUuidStr.empty()) {
            errMsg.addAttribute("uuid", componentUuidStr);
        }
        if(!componentName.empty()) {
            errMsg.addAttribute("name", componentName);
        }
        throw ConfigurationException(errMsg.write());
    }
}

void Registry::declareModule(tinyxml2::XMLElement * xmlModuleElt)
{
    std::string moduleName = "";
    std::string moduleUuidStr = "";
    bool subElementError = false;
    try {
        SPtr<ModuleMetadata> moduleInfo;
        if(xmlModuleElt->Attribute("name")!= nullptr) {
            moduleName = xmlModuleElt->Attribute("name");
        }
        std::string moduleDescription = "";
        if (xmlModuleElt->Attribute("description") != nullptr) {
            moduleDescription = xmlModuleElt->Attribute("description");
        }
        uuids::uuid moduleUuid;
        if(xmlModuleElt->Attribute("uuid")!=nullptr) {
            moduleUuidStr = xmlModuleElt->Attribute("uuid");
        }
        try {
            moduleUuid = toUUID(moduleUuidStr);
            subElementError = true;
        }
        catch(const std::runtime_error &) {
            std::string what = "UUID format invalid in \"uuid\": ";
            what.append(xmlModuleElt->Attribute("uuid"));
            throw ConfigurationException( what );
        }
        moduleInfo = utils::make_shared<ModuleMetadata>(moduleName.c_str(), moduleUuid, moduleDescription.c_str(), xmlModuleElt->Attribute("path"));
        fs::path filePath = moduleInfo->getFullPath();
        // TODO : check lib existenz with file decorations
        /* if ( !boost::filesystem::exists( filePath ) )
        {
            std::cout<<"Error : Module not found at : "<<filePath.c_str()<<std::endl;
            return XPCFErrorCode::_FAIL;
        }*/

        if (! mapContains(m_context->modulesMap, moduleUuid)) {
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
    catch (const xpcf::Exception& e) {
        XmlErrorMessage errMsg("module", e.what(), subElementError);
        if(!moduleUuidStr.empty()) {
            errMsg.addAttribute("uuid", moduleUuidStr);
        }
        if(!moduleName.empty()) {
            errMsg.addAttribute("name", moduleName);
        }
        throw ConfigurationException(errMsg.write());
    }
}

const IEnumerable<SPtr<ModuleMetadata>> & Registry::getModulesMetadata() const
{
    return m_context->modulesVector;
}

const IEnumerable<SPtr<InterfaceMetadata>> & Registry::getInterfacesMetadata() const
{
    return m_context->interfacesVector;
}

}}} //namespace org::bcom::xpcf
