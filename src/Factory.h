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

#ifndef ORG_BCOM_XPCF_FACTORY_H
#define ORG_BCOM_XPCF_FACTORY_H

//#define BOOST_ALL_DYN_LINK 1
#include <xpcf/api/IComponentManager.h>
#include "AliasManager.h"
#include "Registry.h"
#include "PropertyManager.h"
#include <xpcf/component/ComponentBase.h>
#include "tinyxmlhelper.h"

#include <thread>
#include <vector>
#include <map>

namespace org { namespace bcom { namespace xpcf {
enum class ContextType {
    Component,
    Named
};

using FactoryContext = std::pair<ContextType, std::string>;

class IFactory : public virtual IComponentIntrospect {
public:
    virtual ~IFactory() override = default;
    virtual void clear() = 0;
    virtual void autobind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID) = 0;
    virtual void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                      IComponentManager::Scope scope = IComponentManager::Scope::Transient) = 0;
    virtual void bind(const std::string & name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                      IComponentManager::Scope scope = IComponentManager::Scope::Transient) = 0;
    virtual void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                      const uuids::uuid & instanceUUID, IComponentManager::Scope scope = IComponentManager::Scope::Transient) = 0;
    virtual void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
                      const uuids::uuid & instanceUUID, IComponentManager::Scope scope = IComponentManager::Scope::Transient) = 0;
    virtual void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                           const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                           IComponentManager::Scope scope = IComponentManager::Scope::Transient) = 0;
    virtual void bind(const char * name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                           const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                           IComponentManager::Scope scope = IComponentManager::Scope::Transient) = 0;

    virtual void declareFactory(tinyxml2::XMLElement * xmlModuleElt) = 0;
    virtual bool bindExists(const uuids::uuid & interfaceUUID) = 0;
    virtual bool bindExists(const uuids::uuid & interfaceUUID, const std::string & name) = 0;
    virtual bool bindExists(const SPtr<InjectableMetadata> & injectableInfo) = 0;
    virtual bool explicitBindExists(const uuids::uuid & componentUUID) = 0;
    virtual SRef<IComponentIntrospect> resolve(const SPtr<InjectableMetadata> & injectableInfo,
                                               std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID,
                                               std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID, const std::string & name,
                                               std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID) = 0;
    virtual uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID, const std::string & name) = 0;

    template <typename I> SRef<I> resolve();
    template <typename I> SRef<I> resolve(const std::string & name);
    template < typename I, typename C, IComponentManager::Scope scope = IComponentManager::Scope::Transient > void bindLocal();
    template < typename I, typename C, IComponentManager::Scope scope = IComponentManager::Scope::Transient > void bindLocal(const char * name);


};


template <typename I>
SRef<I> IFactory::resolve()
{
    return resolve(toUUID<I>())->template bindTo<I>();
}

template <typename I>
SRef<I> IFactory::resolve(const std::string & name)
{
    return resolve(toUUID<I>(), name)->template bindTo<I>();
}

template < typename I, typename C, IComponentManager::Scope scope> void IFactory::bindLocal()
{
    bind(toUUID<I>(), toUUID<C>(), &ComponentFactory::create<C>, scope);
}

template < typename I, typename C, IComponentManager::Scope scope> void IFactory::bindLocal(const char * name)
{
    bind(name, toUUID<I>(), toUUID<C>(), &ComponentFactory::create<C>, scope);
}

template <> struct InterfaceTraits<IFactory>
{
    static constexpr const char * UUID = "701F105A-C2A1-4939-96D9-754C9A087144";
    static constexpr const char * NAME = "XPCF::IFactory";
    static constexpr const char * DESCRIPTION = "Factory interface.\nProvides binding between interfaces uuid and concrete components uuid for injection patterns";
};

class Factory : public ComponentBase,
        public virtual IFactory {
public:
    Factory();
    ~Factory() override = default;
    void clear() override;
    void autobind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID) override;
    void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
              IComponentManager::Scope scope = IComponentManager::Scope::Transient) override;
    void bind(const std::string & name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
              IComponentManager::Scope scope = IComponentManager::Scope::Transient) override;
    void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
              const uuids::uuid & instanceUUID, IComponentManager::Scope scope = IComponentManager::Scope::Transient) override;
    void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
              const uuids::uuid & instanceUUID, IComponentManager::Scope scope = IComponentManager::Scope::Transient) override;
    void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   IComponentManager::Scope scope = IComponentManager::Scope::Transient) override;
    void bind(const char * name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   IComponentManager::Scope scope = IComponentManager::Scope::Transient) override;

    void declareFactory(tinyxml2::XMLElement * xmlModuleElt) override;
    bool bindExists(const uuids::uuid & interfaceUUID) override;
    bool bindExists(const uuids::uuid & interfaceUUID, const std::string & name) override;
    bool bindExists(const SPtr<InjectableMetadata> & injectableInfo) override;
    bool explicitBindExists(const uuids::uuid & componentUUID) override;
    SRef<IComponentIntrospect> resolve(const SPtr<InjectableMetadata> & injectableInfo,
                                       std::deque<FactoryContext> contextLevels) override;
    SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID,
                                       std::deque<FactoryContext> contextLevels) override;
    SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID, const std::string & name,
                                       std::deque<FactoryContext> contextLevels) override;
    uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID) override;
    uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID, const std::string & name) override;
    void unloadComponent () override final;

private:
    void declareBindings(tinyxml2::XMLElement * xmlModuleElt);
    void declareInjects(tinyxml2::XMLElement * xmlModuleElt);
    void declareBind(tinyxml2::XMLElement * xmlBindElt);
    void declareInject(tinyxml2::XMLElement * xmlBindElt);
    void declareSpecificBind(tinyxml2::XMLElement * xmlBindElt, const uuids::uuid & targetComponentUUID);
    SRef<IComponentIntrospect> resolveFromModule(const uuids::uuid & componentUUID);
    SRef<IComponentIntrospect> resolveComponent(const uuids::uuid & componentUUID,
                                                std::deque<FactoryContext> contextLevels);
    void inject(SRef<IInjectable> component,
                std::deque<FactoryContext> contextLevels);
    SPtr<ModuleMetadata> resolveMetadataFromComponentUUID(const uuids::uuid & componentUUID) {
        return m_resolver->findModuleMetadata(m_resolver->getModuleUUID(componentUUID));
    }
    std::pair<uuids::uuid, IComponentManager::Scope> resolveBind(const uuids::uuid & interfaceUUID, std::deque<FactoryContext> contextLevels);
    std::pair<uuids::uuid, IComponentManager::Scope> resolveBind(const uuids::uuid & interfaceUUID, const std::string & name, std::deque<FactoryContext> contextLevels );

    //boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;
    // interface Uuid resolves to [ component Uuid , scope ]
    std::map<uuids::uuid, std::pair<uuids::uuid, IComponentManager::Scope>> m_autoBindings;
    std::map<uuids::uuid, std::pair<uuids::uuid, IComponentManager::Scope>> m_defaultBindings;

    // [interface Uuid, name] resolves to [ component Uuid , scope ]
    std::map<std::pair<uuids::uuid,std::string>, std::pair<uuids::uuid, IComponentManager::Scope> > m_namedBindings;

    // component UUID resolves to create function
    std::map<uuids::uuid, std::function<SRef<IComponentIntrospect>(void)>> m_factoryMethods;

    // component Uuid resolves to IComponentIntrospect reference
    std::map<uuids::uuid,SRef<IComponentIntrospect> > m_singletonInstances;

    // [component Uuid, name] resolves to IComponentIntrospect reference
    std::map<std::pair<uuids::uuid,std::string>,SRef<IComponentIntrospect> > m_namedSingletonInstances;

    // component Uuid resolves to [ Interface, [component Uuid , scope] ]
    // Note : when a specific binding already exists in defaultBindings, it is not added in m_specificBindings
    std::map<uuids::uuid, std::map<uuids::uuid, std::pair<uuids::uuid, IComponentManager::Scope>> > m_specificBindings;

    // component Uuid resolves to [ [interface Uuid , name] , [component Uuid , scope] ]
    std::map<uuids::uuid, std::map<std::pair<uuids::uuid,std::string>, std::pair<uuids::uuid, IComponentManager::Scope>> > m_specificNamedBindings;
    SRef<IRegistry> m_resolver;
    SRef<IAliasManager> m_aliasManager;
    SRef<IPropertyManager> m_propertyManager;
};

template <> struct ComponentTraits<Factory>
{
    static constexpr const char * UUID = "D6B80168-5E3D-4CB8-B7DF-1261FFE67E9E";
    static constexpr const char * NAME = "XPCF::Factory";
    static constexpr const char * DESCRIPTION = "XPCF::Factory";
};


}}} //namespace org::bcom::xpcf

#endif
