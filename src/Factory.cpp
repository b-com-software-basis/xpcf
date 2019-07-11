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
 * @date 2019-06-05
 */


#include "Factory.h"
#include <xpcf/api/IModuleManager.h>
#include <xpcf/core/Exception.h>
#include <xpcf/core/helpers.h>

using namespace std;
using placeholders::_1;
namespace fs = boost::filesystem;

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(org::bcom::xpcf::Factory);

namespace org { namespace bcom { namespace xpcf {

constexpr const char * XMLBINDINGSNODE = "bindings";
constexpr const char * XMLBINDNODE = "bind";
constexpr const char * XMLINJECTSNODE = "injects";
constexpr const char * XMLSCOPENODE = "scope";

static const map<string,IComponentManager::Scope> scopeConvertMap = {
    {"Transient",IComponentManager::Scope::Transient},
    {"Singleton",IComponentManager::Scope::Singleton},
};
Factory::Factory():ComponentBase(toUUID<Factory>())
{
    declareInterface<IFactory>(this);
    declareInjectable<IAliasManager>(m_aliasManager);
    declareInjectable<IRegistry>(m_resolver);
}

void Factory::clear()
{
    m_autoBindings.clear();
    m_defaultBindings.clear();
    m_specificBindings.clear();
    m_specificNamedBindings.clear();
    m_singletonInstances.clear();
    m_namedBindings.clear();
}

void Factory::declareBind(tinyxml2::XMLElement * xmlBindElt)
{
    if ((xmlBindElt->Attribute("interface") == nullptr) || (xmlBindElt->Attribute("to") == nullptr)) {
        throw Exception("<bind interface=\"...\"  to=\"...\"/> node incomplete : attributes 'interface' and 'to' are mandatory.");
    }
    string interfaceAttrValue =  xmlBindElt->Attribute("interface");
    uuids::uuid interfaceUUID;
    if (m_aliasManager->aliasExists(IAliasManager::Type::Interface, interfaceAttrValue)) {
        interfaceUUID = m_aliasManager->resolveInterfaceAlias(interfaceAttrValue);
    }
    else {
        interfaceUUID =  toUUID(interfaceAttrValue);
    }
    uuids::uuid componentUUID;
    string componentAttrValue =  xmlBindElt->Attribute("to");
    if (m_aliasManager->aliasExists(IAliasManager::Type::Component, componentAttrValue)) {
        componentUUID = m_aliasManager->resolveComponentAlias(componentAttrValue);
    }
    else {
        componentUUID =  toUUID(componentAttrValue);
    }
    const char * scopeType =  xmlBindElt->Attribute("scope");
    // Set default scope to Transient
    IComponentManager::Scope scope = IComponentManager::Scope::Transient;
    if (scopeType != nullptr) {
        scope = scopeConvertMap.at(scopeType);
    }
    if (xmlBindElt->Attribute("name") == nullptr) {
        bind(interfaceUUID, componentUUID, scope);
    }
    else {
        string nameAttrValue =  xmlBindElt->Attribute("name");
        bind(nameAttrValue, interfaceUUID, componentUUID, scope);
    }
}

void Factory::declareBindings(tinyxml2::XMLElement * xmlBindingsElt)
{
    processXmlNode(xmlBindingsElt, XMLBINDNODE, std::bind(&Factory::declareBind, this, _1));
}

void Factory::declareInjects(tinyxml2::XMLElement * xmlInjectsElt)
{
}

void Factory::declareFactory(tinyxml2::XMLElement * xmlFactoryElt)
{
    processXmlNode(xmlFactoryElt, XMLBINDINGSNODE, std::bind(&Factory::declareBindings, this, _1));
    processXmlNode(xmlFactoryElt, XMLINJECTSNODE, std::bind(&Factory::declareInjects, this, _1));
}

void Factory::autobind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID)
{
    if (! mapContains(m_defaultBindings,interfaceUUID)) {
        // no explicit bind already exists : add autobind
        m_autoBindings[interfaceUUID] = make_pair(instanceUUID,IComponentManager::Scope::Transient);
    }
}

void Factory::bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID, IComponentManager::Scope scope)
{
    if (mapContains(m_defaultBindings,interfaceUUID)) {
        // bind already exists : error ???
    }
    if (mapContains(m_autoBindings,interfaceUUID)) {
        // remove autobind as an explicit bind is added
        m_autoBindings.erase(interfaceUUID);
    }
    m_defaultBindings[interfaceUUID] = make_pair(instanceUUID,scope);
}

void Factory::bind(const std::string & name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID, IComponentManager::Scope scope)
{
    pair<uuids::uuid,string> key = make_pair(interfaceUUID,name);
    if (mapContains(m_namedBindings,key)) {
        // bind already exists : error ???
    }
    m_namedBindings[key] = make_pair(instanceUUID,scope);
}

bool Factory::bindExists(const uuids::uuid & interfaceUUID)
{
    bool bindingExists = ( mapContains(m_defaultBindings,interfaceUUID) || mapContains(m_autoBindings,interfaceUUID) );
    if (!bindingExists) {
       /* pair<uuids::uuid,string> key = make_pair(interfaceUUID, injectableInfo->getName());
        bindingExists = bindingExists || (m_namedBindings.find(key) != m_namedBindings.end());*/
    }
    return bindingExists;
}

bool Factory::bindExists(const uuids::uuid & interfaceUUID, const string & name)
{
    pair<uuids::uuid,string> key = make_pair(interfaceUUID,name);
    return (mapContains(m_namedBindings,key));
}


bool Factory::bindExists(const SPtr<InjectableMetadata> & injectableInfo)
{
    bool bindingExists = ( mapContains(m_defaultBindings,injectableInfo->getUuid()) || mapContains(m_autoBindings,injectableInfo->getUuid()) );
    if (injectableInfo->isNamed()) {
        pair<uuids::uuid, string> key = make_pair(injectableInfo->getUuid(), injectableInfo->getName());
        bindingExists = bindingExists || mapContains(m_namedBindings,key);
    }
    return bindingExists;
}

std::pair<uuids::uuid, IComponentManager::Scope> Factory::resolveBind(const uuids::uuid & interfaceUUID)
{
    if (mapContains(m_defaultBindings,interfaceUUID)) {
       pair<uuids::uuid,IComponentManager::Scope> bindInfos = m_defaultBindings.at(interfaceUUID);
       return bindInfos;
    }
    if (mapContains(m_autoBindings,interfaceUUID)) {
        return m_autoBindings.at(interfaceUUID);
    }
    throw InjectableNotFoundException("No [auto|default|named] binding found for injectable");
}

std::pair<uuids::uuid, IComponentManager::Scope> Factory::resolveBind(const uuids::uuid & interfaceUUID, const std::string & name)
{
    pair<uuids::uuid, string> key = make_pair(interfaceUUID, name);
    if (mapContains(m_namedBindings,key)) {
       pair<uuids::uuid,IComponentManager::Scope> bindInfos = m_namedBindings.at(key);
       return bindInfos;
    }
    return resolveBind(interfaceUUID);
}

uuids::uuid Factory::getComponentUUID(const uuids::uuid & interfaceUUID)
{
    pair<uuids::uuid,IComponentManager::Scope> bindInfos = resolveBind(interfaceUUID);
    return bindInfos.first;
}

uuids::uuid Factory::getComponentUUID(const uuids::uuid & interfaceUUID, const std::string & name)
{
    pair<uuids::uuid,IComponentManager::Scope> bindInfos = resolveBind(interfaceUUID, name);
    return bindInfos.first;
}

SRef<IComponentIntrospect> Factory::resolve(const uuids::uuid & interfaceUUID)
{
    pair<uuids::uuid,IComponentManager::Scope> bindInfos = resolveBind(interfaceUUID);
    uuids::uuid componentUUID = bindInfos.first;
    SPtr<ModuleMetadata> moduleInfos = resolveMetadataFromComponentUUID(componentUUID);
    if (bindInfos.second == IComponentManager::Scope::Singleton) {
        if (! mapContains(m_singletonInstances,interfaceUUID)) {
            SRef<IComponentIntrospect> componentRef = getModuleManagerInstance()->createComponent(moduleInfos, componentUUID);
            m_singletonInstances[interfaceUUID] = componentRef;
        }
        return m_singletonInstances.at(interfaceUUID);
    }
    return getModuleManagerInstance()->createComponent(moduleInfos, componentUUID);
}

SRef<IComponentIntrospect> Factory::resolve(const uuids::uuid & interfaceUUID, const string & name)
{
    pair<uuids::uuid, IComponentManager::Scope> bindInfos = resolveBind(interfaceUUID, name);
    uuids::uuid componentUUID = bindInfos.first;
    SPtr<ModuleMetadata> moduleInfos = resolveMetadataFromComponentUUID(componentUUID);
    if (bindInfos.second == IComponentManager::Scope::Singleton) {
        pair<uuids::uuid,string> key = make_pair(interfaceUUID, name);
        if (! mapContains(m_namedSingletonInstances,key)) {
            SRef<IComponentIntrospect> componentRef = getModuleManagerInstance()->createComponent(moduleInfos, componentUUID);
            m_namedSingletonInstances[key] = componentRef;
        }
        return m_namedSingletonInstances.at(key);
    }
    return getModuleManagerInstance()->createComponent(moduleInfos, componentUUID);
}


SRef<IComponentIntrospect> Factory::resolve(const SRef<InjectableMetadata> & injectableInfo)
{
    if (injectableInfo->isNamed()) {
        return resolve(injectableInfo->getUuid(), injectableInfo->getName());
    }
    return resolve(injectableInfo->getUuid());
}

}}} //namespace org::bcom::xpcf
