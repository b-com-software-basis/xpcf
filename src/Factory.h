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
#include <xpcf/collection/Collection.h>

#ifdef XPCF_WITH_LOGS
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#endif

#include <thread>
#include <vector>
#include <map>

namespace org { namespace bcom { namespace xpcf {
enum class ContextType {
    Component,
    Named,
    Specific,
    Multi
};

struct FactoryBindInfos {
    uuids::uuid componentUUID;
    BindingScope scope;
    uint8_t bindingRangeMask = 0;
    std::string properties;
};

using FactoryContext = std::pair<ContextType, FactoryBindInfos>;

inline bool operator==(const FactoryBindInfos& lhs, const FactoryBindInfos& rhs)
{
    return lhs.componentUUID == rhs.componentUUID &&
           lhs.scope == rhs.scope &&
           lhs.properties == rhs.properties;
}

class IFactory : virtual public IComponentIntrospect {
public:
    virtual ~IFactory() override = default;
    virtual void clear() = 0;
    virtual void autobind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID) = 0;
    virtual void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                      BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const std::string & name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                      BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                      const uuids::uuid & instanceUUID, BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
                      const uuids::uuid & instanceUUID, BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                           const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                           BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const std::string & name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                           const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                           BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                      const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                      const uuids::uuid & instanceUUID, BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
                      const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                      const uuids::uuid & instanceUUID, BindingScope scope,
                      uint8_t bindingRangeMask) = 0;

    virtual void declareFactory(tinyxml2::XMLElement * xmlModuleElt) = 0;
    virtual SRef<IComponentIntrospect> resolve(const SPtr<InjectableMetadata> & injectableInfo,
                                               std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID,
                                               std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID, const std::string & name,
                                               std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const SPtr<InjectableMetadata> & injectableInfo,
                                               std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const uuids::uuid & interfaceUUID,
                                       std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID) = 0;
    virtual uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID, const std::string & name) = 0;

    virtual void inject(SRef<IInjectable> component, std::deque<FactoryContext> contextLevels = {}) = 0;
    template <typename I> SRef<I> resolve();
    template <typename I> SRef<I> resolve(const std::string & name);
    template < typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Default|BindingRange::All  > void bindLocal();
    template < typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Named  > void bindLocal(const char * name);
    template < typename T, typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit > void bindLocal();
    template < typename T, typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit > void bindLocal(const char * name);


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

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IFactory::bindLocal()
{
    bind(toUUID<I>(), toUUID<C>(), &ComponentFactory::create<C>, scope, bindingRangeMask);
}

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IFactory::bindLocal(const char * name)
{
    bind(name, toUUID<I>(), toUUID<C>(), &ComponentFactory::create<C>, scope, bindingRangeMask);
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void bindLocal()
{
    bind(toUUID<T>(), toUUID<I>(), toUUID<C>(), &ComponentFactory::create<C>, scope, bindingRangeMask);
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void bindLocal(const char * name)
{
    bind(toUUID<T>(), name, toUUID<C>(), &ComponentFactory::create<C>, scope, bindingRangeMask);
}


template <> struct InterfaceTraits<IFactory>
{
    static constexpr const char * UUID = "701F105A-C2A1-4939-96D9-754C9A087144";
    static constexpr const char * NAME = "XPCF::IFactory";
    static constexpr const char * DESCRIPTION = "Factory interface.\nProvides binding between interfaces uuid and concrete components uuid for injection patterns";
};

class Factory : public ComponentBase,
        virtual public IFactory {
public:
    Factory();
    ~Factory() override = default;
    void clear() override;
    void autobind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID) override;
    void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
              BindingScope scope,
              uint8_t bindingRangeMask) override;
    void bind(const std::string & name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
              BindingScope scope,
              uint8_t bindingRangeMask) override;
    void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
              const uuids::uuid & instanceUUID, BindingScope scope,
              uint8_t bindingRangeMask) override;
    void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
              const uuids::uuid & instanceUUID, BindingScope scope,
              uint8_t bindingRangeMask) override;
    void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   BindingScope scope,
              uint8_t bindingRangeMask) override;
    void bind(const std::string & name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   BindingScope scope,
              uint8_t bindingRangeMask) override;
    void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
              const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
              const uuids::uuid & instanceUUID, BindingScope scope,
              uint8_t bindingRangeMask) override;
    void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
              const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
              const uuids::uuid & instanceUUID, BindingScope scope,
              uint8_t bindingRangeMask) override;

    void bind(const uuids::uuid & interfaceUUID, const FactoryBindInfos & bindInfos);
    void bind(const std::string & name, const uuids::uuid & interfaceUUID, const FactoryBindInfos & bindInfos);
    void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID, const FactoryBindInfos & bindInfos);
    void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
              const FactoryBindInfos & bindInfos);
    void bind(const uuids::uuid & interfaceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   const FactoryBindInfos & bindInfos);
    void bind(const std::string & name, const uuids::uuid & interfaceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   const FactoryBindInfos & bindInfos);
    void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   const FactoryBindInfos & bindInfos);
    void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   const FactoryBindInfos & bindInfos);

    void declareFactory(tinyxml2::XMLElement * xmlModuleElt) override;
    SRef<IComponentIntrospect> resolve(const SPtr<InjectableMetadata> & injectableInfo,
                                       std::deque<FactoryContext> contextLevels) override;
    SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID,
                                       std::deque<FactoryContext> contextLevels) override;
    SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID, const std::string & name,
                                       std::deque<FactoryContext> contextLevels) override;
    const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const SPtr<InjectableMetadata> & injectableInfo,
                                                   std::deque<FactoryContext> contextLevels = {}) override;
    const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const uuids::uuid & interfaceUUID,
                                       std::deque<FactoryContext> contextLevels) override;
    uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID) override;
    uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID, const std::string & name) override;
    void unloadComponent () override final;

private:
    void declareBindings(tinyxml2::XMLElement * xmlModuleElt);
    void declareInjects(tinyxml2::XMLElement * xmlModuleElt);
    void declareBind(tinyxml2::XMLElement * xmlBindElt);
    void declareSingleBind(const uuids::uuid & interfaceUUID, tinyxml2::XMLElement * xmlBindElt);
    void declareMultiBind(const uuids::uuid & interfaceUUID, tinyxml2::XMLElement * xmlBindElt);
    void declareInject(tinyxml2::XMLElement * xmlBindElt);
    void declareSpecificBind(tinyxml2::XMLElement * xmlBindElt, const uuids::uuid & targetComponentUUID);
    FactoryBindInfos getComponentBindingInfos(tinyxml2::XMLElement * xmlBindElt);
    SRef<IComponentIntrospect> resolveFromModule(const uuids::uuid & componentUUID);
    SRef<IComponentIntrospect> resolveComponent(const FactoryBindInfos & bindInfos,
                                                std::deque<FactoryContext> contextLevels);
    void inject(SRef<IInjectable> component,
                std::deque<FactoryContext> contextLevels) final;
    SPtr<ModuleMetadata> resolveMetadataFromComponentUUID(const uuids::uuid & componentUUID) {
        return m_resolver->findModuleMetadata(m_resolver->getModuleUUID(componentUUID));
    }
    FactoryBindInfos resolveBind(const uuids::uuid & interfaceUUID, std::deque<FactoryContext> contextLevels);
    FactoryBindInfos resolveBind(const uuids::uuid & interfaceUUID, const std::string & name,  std::deque<FactoryContext> contextLevels );
#ifdef XPCF_WITH_LOGS
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;
#endif

    // interface Uuid resolves to [ component Uuid , scope ]
    std::map<uuids::uuid, FactoryBindInfos> m_autoBindings;
    std::map<uuids::uuid, FactoryBindInfos> m_defaultBindings;
    std::map<uuids::uuid, VectorCollection<FactoryBindInfos>> m_multiBindings;

    // default and multibind properties association
    // [interface UUID, component UUID] => properties name
    std::map<std::pair<uuids::uuid, uuids::uuid>, std::string > m_bindingsProperties;
    std::map<std::pair<uuids::uuid, uuids::uuid>, std::string > m_multiBindingsProperties;


    // [interface Uuid, name] resolves to [ component Uuid , scope ]
    std::map<std::pair<uuids::uuid,std::string>, FactoryBindInfos > m_namedBindings;
    std::map<std::pair<uuids::uuid,std::string>, std::string > m_namedBindingsProperties;

    // component UUID resolves to create function
    std::map<uuids::uuid, std::function<SRef<IComponentIntrospect>(void)>> m_factoryMethods;

    // component Uuid resolves to IComponentIntrospect reference
    std::map<uuids::uuid,SRef<IComponentIntrospect> > m_singletonInstances;

    // [component Uuid, name] resolves to IComponentIntrospect reference
    std::map<std::pair<uuids::uuid,std::string>,SRef<IComponentIntrospect> > m_namedSingletonInstances;

    // component Uuid resolves to [ Interface, [component Uuid , scope] ]
    // Note : when a specific binding already exists in defaultBindings, it is not added in m_specificBindings
    std::map<uuids::uuid, std::map<uuids::uuid, FactoryBindInfos> > m_specificBindings;

    // component Uuid resolves to [ [interface Uuid , name] , [component Uuid , scope] ]
    std::map<uuids::uuid, std::map<std::pair<uuids::uuid,std::string>, FactoryBindInfos> > m_specificNamedBindings;
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
