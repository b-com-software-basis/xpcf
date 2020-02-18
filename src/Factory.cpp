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
using placeholders::_2;
namespace fs = boost::filesystem;

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(org::bcom::xpcf::Factory);

namespace org { namespace bcom { namespace xpcf {

constexpr const char * XMLBINDINGSNODE = "bindings";
constexpr const char * XMLBINDNODE = "bind";
constexpr const char * XMLINJECTSNODE = "injects";
constexpr const char * XMLINJECTNODE = "inject";
constexpr const char * XMLSCOPENODE = "scope";

static const map<string,IComponentManager::Scope> scopeConvertMap = {
    {"Transient",IComponentManager::Scope::Transient},
    {"Singleton",IComponentManager::Scope::Singleton},
};

Factory::Factory():ComponentBase(toUUID<Factory>())
{
    declareInterface<IFactory>(this);
    IFactory::bindLocal<IAliasManager,AliasManager,IComponentManager::Scope::Singleton>();
    IFactory::bindLocal<IRegistry,Registry,IComponentManager::Scope::Singleton>();
    bind(toUUID<IPropertyManager>(), toUUID<PropertyManager>(), &getPropertyManagerInstance, IComponentManager::Scope::Singleton);
    m_propertyManager = IFactory::resolve<IPropertyManager>();
    m_aliasManager = IFactory::resolve<IAliasManager>();
    m_resolver = IFactory::resolve<IRegistry>();
    std::function<void(const uuids::uuid &, const uuids::uuid &)> bindFunc = [&] (const uuids::uuid & interfaceUUID, const uuids::uuid & componentUUID) -> void {
        autobind(interfaceUUID,componentUUID);
    };
    m_resolver->setBinder(bindFunc);
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

void Factory::declareSpecificBind(tinyxml2::XMLElement * xmlBindElt, const uuids::uuid & targetComponentUUID)
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
        bind(targetComponentUUID, interfaceUUID, componentUUID, scope);
    }
    else {
        string nameAttrValue =  xmlBindElt->Attribute("name");
        bind(targetComponentUUID, nameAttrValue, interfaceUUID, componentUUID, scope);
    }
}

void Factory::declareBindings(tinyxml2::XMLElement * xmlBindingsElt)
{
    processXmlNode(xmlBindingsElt, XMLBINDNODE, std::bind(&Factory::declareBind, this, _1));
}

void Factory::declareInject(tinyxml2::XMLElement * xmlInjectElt)
{
    // Parse bindings : if binding already exist in default or namedBindings: ignore it
    // for each non-existent bind, add the specificBinding
    if (xmlInjectElt->Attribute("to") == nullptr) {
        throw Exception("<inject  to=\"...\"/> node incomplete : attribute 'to' is mandatory.");
    }
    uuids::uuid componentUUID;
    string componentAttrValue =  xmlInjectElt->Attribute("to");
    if (m_aliasManager->aliasExists(IAliasManager::Type::Component, componentAttrValue)) {
        componentUUID = m_aliasManager->resolveComponentAlias(componentAttrValue);
    }
    else {
        componentUUID =  toUUID(componentAttrValue);
    }
    std::function<void(tinyxml2::XMLElement*,  const uuids::uuid &)> declareSpecificBindingsFunc = std::bind(&Factory::declareSpecificBind, this, _1,_2);
    processXmlNode<const uuids::uuid &>(xmlInjectElt, XMLBINDNODE, declareSpecificBindingsFunc, componentUUID);
}

void Factory::declareInjects(tinyxml2::XMLElement * xmlInjectsElt)
{
    processXmlNode(xmlInjectsElt, XMLINJECTNODE, std::bind(&Factory::declareInject, this, _1));
}

void Factory::declareFactory(tinyxml2::XMLElement * xmlFactoryElt)
{
    processXmlNode(xmlFactoryElt, XMLBINDINGSNODE, std::bind(&Factory::declareBindings, this, _1));
    processXmlNode(xmlFactoryElt, XMLINJECTSNODE, std::bind(&Factory::declareInjects, this, _1));
}

SRef<IComponentIntrospect> Factory::resolveFromModule(const uuids::uuid & componentUUID)
{
    SPtr<ModuleMetadata> moduleInfos = resolveMetadataFromComponentUUID(componentUUID);
    SRef<IComponentIntrospect> componentRef = getModuleManagerInstance()->createComponent(moduleInfos, componentUUID);
    return componentRef;
}

void Factory::autobind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID)
{
    if (! mapContains(m_defaultBindings,interfaceUUID)) {
        // no explicit bind already exists : add autobind
        m_autoBindings[interfaceUUID] = make_pair(instanceUUID,IComponentManager::Scope::Transient);
    }
}

void Factory::bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                   IComponentManager::Scope scope)
{
    if (mapContains(m_defaultBindings,interfaceUUID)) {
        // bind already exists : error ???
        // should we return or update the bind ?
    }
    if (mapContains(m_autoBindings,interfaceUUID)) {
        // remove autobind as an explicit bind is added
        m_autoBindings.erase(interfaceUUID);
    }
    m_defaultBindings[interfaceUUID] = make_pair(instanceUUID,scope);
}

void Factory::bind(const std::string & name, const uuids::uuid & interfaceUUID,
                   const uuids::uuid & instanceUUID, IComponentManager::Scope scope)
{
    pair<uuids::uuid,string> key = make_pair(interfaceUUID,name);
    if (mapContains(m_namedBindings,key)) {
        // bind already exists : error ???
    }
    m_namedBindings[key] = make_pair(instanceUUID,scope);
}

void Factory::bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                        const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                        IComponentManager::Scope scope)
{
    bind(interfaceUUID, instanceUUID, scope);
    m_factoryMethods[instanceUUID] = factoryFunc;
}

void Factory::bind(const char * name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                              const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                              IComponentManager::Scope scope)
{
    bind(name, interfaceUUID, instanceUUID, scope);
    m_factoryMethods[instanceUUID] = factoryFunc;
}


void Factory::bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                   const uuids::uuid & instanceUUID, IComponentManager::Scope scope)
{
    if (mapContains(m_specificBindings,targetComponentUUID)) {
        // bind already exists : error ???
    }
    m_specificBindings[targetComponentUUID][interfaceUUID] = make_pair(instanceUUID,scope);
}

void Factory::bind(const uuids::uuid & targetComponentUUID, const std::string & name,
                   const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                   IComponentManager::Scope scope)
{
    if (mapContains(m_specificNamedBindings,targetComponentUUID)) {
        // bind already exists : error ???
    }
    m_specificNamedBindings[targetComponentUUID][ make_pair(interfaceUUID,name)] = make_pair(instanceUUID,scope);
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

bool Factory::explicitBindExists(const uuids::uuid & componentUUID)
{
    return mapContains(m_specificBindings, componentUUID);
}

std::pair<uuids::uuid, IComponentManager::Scope> Factory::resolveBind(const uuids::uuid & interfaceUUID, std::deque<FactoryContext> contextLevels)
{
    // search for the closest specific declaration of binding for interfaceUUID : the first context is the last one, the latter is the first specific context created in the tree
    for (auto [contextType,contextValue] : contextLevels) {
        if (contextType == ContextType::Component) {
            auto componentUUID = toUUID(contextValue);
            if (mapContains(m_specificBindings, componentUUID)) {
                if (mapContains(m_specificBindings[componentUUID], interfaceUUID)) {
                    pair<uuids::uuid,IComponentManager::Scope> bindInfos = m_specificBindings[componentUUID][interfaceUUID];
                    return bindInfos;
                }
            }
        }
        //TODO : handle named contexts
    }
    // no specific binding found for this interface in contexts : search for a default binding
    if (mapContains(m_defaultBindings, interfaceUUID)) {
        pair<uuids::uuid,IComponentManager::Scope> bindInfos = m_defaultBindings.at(interfaceUUID);
        return bindInfos;
    }
    // no default binding found for this interface : search for an autobinding
    if (mapContains(m_autoBindings,interfaceUUID)) {
        return m_autoBindings.at(interfaceUUID);
    }
    throw InjectableNotFoundException("No [auto|default|named] binding found for injectable");
}

std::pair<uuids::uuid, IComponentManager::Scope> Factory::resolveBind(const uuids::uuid & interfaceUUID, const std::string & name, std::deque<FactoryContext> contextLevels)
{
     pair<uuids::uuid, string> key = make_pair(interfaceUUID, name);
    // search for the closest specific declaration of binding for interfaceUUID : the first context is the last one, the latter is the first specific context created in the tree
    for (auto [contextType,contextValue] : contextLevels) {
        if (contextType == ContextType::Component) {
            auto componentUUID = toUUID(contextValue);
            if (mapContains(m_specificNamedBindings, componentUUID)) {
                if (mapContains(m_specificNamedBindings[componentUUID], key)) {
                    pair<uuids::uuid,IComponentManager::Scope> bindInfos = m_specificNamedBindings[componentUUID][key];
                    return bindInfos;
                }
            }
        }
        //TODO : handle named contexts
    }
    // no specific named binding found for this interface in contexts : search for a default named binding
    if (mapContains(m_namedBindings,key)) {
        pair<uuids::uuid,IComponentManager::Scope> bindInfos = m_namedBindings.at(key);
        return bindInfos;
    }
    return resolveBind(interfaceUUID, contextLevels);
}

uuids::uuid Factory::getComponentUUID(const uuids::uuid & interfaceUUID)
{
    pair<uuids::uuid,IComponentManager::Scope> bindInfos = resolveBind(interfaceUUID, {});
    return bindInfos.first;
}

uuids::uuid Factory::getComponentUUID(const uuids::uuid & interfaceUUID, const std::string & name)
{
    pair<uuids::uuid,IComponentManager::Scope> bindInfos = resolveBind(interfaceUUID, name, {});
    return bindInfos.first;
}

void Factory::inject(SRef<IInjectable> component, std::deque<FactoryContext> contextLevels)
{
    for (auto injectable : component->getInjectables()) {
        if (!injectable->optional() && !bindExists(injectable)) {
            throw InjectableNotFoundException(injectable);
        }
        if (bindExists(injectable)) {
            SRef<IComponentIntrospect> injectableRef = resolve(injectable, contextLevels);
            injectable->inject(injectableRef);
        }
    }
    component->onInjected();
}

SRef<IComponentIntrospect> Factory::resolveComponent(const uuids::uuid & componentUUID, std::deque<FactoryContext> contextLevels)
{
    std::function<SRef<IComponentIntrospect>(void)> createComponent = [componentUUID, this]()-> SRef<IComponentIntrospect> {
        return resolveFromModule(componentUUID);
    };

    if (mapContains(m_factoryMethods, componentUUID)) {
        createComponent =  m_factoryMethods[componentUUID];
    }

    SRef<IComponentIntrospect> componentRef = createComponent();
    inject(componentRef->bindTo<IInjectable>(), contextLevels);
    if (componentRef->implements<IConfigurable>()) {
        fs::path configFilePath = m_propertyManager->getConfigPath(componentUUID);
        if (! configFilePath.empty()) {
            SRef<IConfigurable> iconf = componentRef->bindTo<IConfigurable>();
            iconf->configure(configFilePath.string().c_str());
        }
    }
    return componentRef;
}

SRef<IComponentIntrospect> Factory::resolve(const uuids::uuid & interfaceUUID, std::deque<FactoryContext> contextLevels)
{
    pair<uuids::uuid,IComponentManager::Scope> bindInfos = resolveBind(interfaceUUID, contextLevels);
    uuids::uuid componentUUID = bindInfos.first;

    contextLevels.push_front(FactoryContext(ContextType::Component,uuids::to_string(componentUUID)));
    if (bindInfos.second == IComponentManager::Scope::Singleton) {
        if (! mapContains(m_singletonInstances,componentUUID)) {
            SRef<IComponentIntrospect> componentRef = resolveComponent(componentUUID, contextLevels);
            m_singletonInstances[componentUUID] = componentRef;
        }
        return m_singletonInstances.at(componentUUID);
    }
    return resolveComponent(componentUUID, contextLevels);
}

SRef<IComponentIntrospect> Factory::resolve(const uuids::uuid & interfaceUUID, const string & name, std::deque<FactoryContext> contextLevels )
{
    pair<uuids::uuid, IComponentManager::Scope> bindInfos = resolveBind(interfaceUUID, name, contextLevels);
    uuids::uuid componentUUID = bindInfos.first;

    contextLevels.push_front(FactoryContext(ContextType::Component,uuids::to_string(componentUUID)));
    if (bindInfos.second == IComponentManager::Scope::Singleton) {
        pair<uuids::uuid,string> key = make_pair(componentUUID, name);
        if (! mapContains(m_namedSingletonInstances,key)) {
            SRef<IComponentIntrospect> componentRef = resolveComponent(componentUUID, contextLevels);
            m_namedSingletonInstances[key] = componentRef;
        }
        return m_namedSingletonInstances.at(key);
    }
    return resolveComponent(componentUUID, contextLevels);
}


SRef<IComponentIntrospect> Factory::resolve(const SRef<InjectableMetadata> & injectableInfo, std::deque<FactoryContext> contextLevels )
{
    if (injectableInfo->isNamed()) {
        return resolve(injectableInfo->getUuid(), injectableInfo->getName(), contextLevels);
    }
    return resolve(injectableInfo->getUuid(), contextLevels);
}

}}} //namespace org::bcom::xpcf
