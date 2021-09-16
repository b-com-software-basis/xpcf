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
#include <xpcf/api/IFactory.h>
#include <xpcf/component/ComponentFactory.h>

#include <deque>
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

struct FactoryBindInfos {
    uuids::uuid componentUUID;
    BindingScope scope;
    uint8_t bindingRangeMask = 0;
    std::string properties;
};

constexpr uint8_t BindingRange_Core = 0;

struct FactoryContext {
    // interface Uuid resolves to [ component Uuid , scope ]
    std::map<uuids::uuid, FactoryBindInfos> autoBindings;
    std::map<uuids::uuid, FactoryBindInfos> defaultBindings;
    std::map<uuids::uuid, VectorCollection<FactoryBindInfos>> multiBindings;

    // default and multibind properties association
    // [interface UUID, component UUID] => properties name
    std::map<std::pair<uuids::uuid, uuids::uuid>, std::string > bindingsProperties;
    std::map<std::pair<uuids::uuid, uuids::uuid>, std::string > multiBindingsProperties;


    // [interface Uuid, name] resolves to [ component Uuid , scope ]
    std::map<std::pair<uuids::uuid,std::string>, FactoryBindInfos > namedBindings;
    std::map<std::pair<uuids::uuid,std::string>, std::string > namedBindingsProperties;
    // component UUID resolves to create function
    std::map<uuids::uuid, std::function<SRef<IComponentIntrospect>(void)>> factoryMethods;
    // component Uuid resolves to [ Interface, [component Uuid , scope] ]
    // Note : when a specific binding already exists in defaultBindings, it is not added in m_specificBindings
    std::map<uuids::uuid, std::map<uuids::uuid, FactoryBindInfos> > specificBindings;

    // component Uuid resolves to [ [interface Uuid , name] , [component Uuid , scope] ]
    std::map<uuids::uuid, std::map<std::pair<uuids::uuid,std::string>, FactoryBindInfos> > specificNamedBindings;

    void clear();

};

enum class ContextType {
    Component,
    Named,
    Specific,
    Multi
};

using BindContext = std::pair<ContextType, FactoryBindInfos>;

inline bool operator==(const FactoryBindInfos& lhs, const FactoryBindInfos& rhs)
{
    return lhs.componentUUID == rhs.componentUUID &&
           lhs.scope == rhs.scope &&
           lhs.properties == rhs.properties;
}

class AbstractFactory : virtual public IFactory {
public:
    virtual ~AbstractFactory() override = default;
    virtual void autobind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID) = 0;
    virtual void inject(SRef<IInjectable> component, std::deque<BindContext> contextLevels = {}) = 0;
    virtual void declareFactory(tinyxml2::XMLElement * xmlModuleElt) = 0;
};

template <> struct InterfaceTraits<AbstractFactory>
{
    static constexpr const char * UUID = "701F105A-C2A1-4939-96D9-754C9A087144";
    static constexpr const char * NAME = "XPCF::AbstractFactory";
    static constexpr const char * DESCRIPTION = "AbstractFactory interface.\nProvides binding between interfaces uuid and concrete components uuid for injection patterns";
};

class Factory : public ComponentBase,
        virtual public AbstractFactory, virtual public IAliasManager, virtual public IRegistryManager {
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

    XPCFErrorCode load() override;
    XPCFErrorCode load(const char* libraryFilePath) override;
    XPCFErrorCode load(const char* folderPathStr, bool bRecurse) override;

    void declareFactory(tinyxml2::XMLElement * xmlModuleElt) override;
    SRef<IComponentIntrospect> resolve(const SPtr<InjectableMetadata> & injectableInfo) override
        { return resolve(injectableInfo, {}); }
    SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID) override
        { return resolve(interfaceUUID, std::deque<BindContext>()); }
    SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID, const std::string & name) override
        { return resolve(interfaceUUID, name, {}); }
    const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const SPtr<InjectableMetadata> & injectableInfo) override
        { return resolveAll(injectableInfo, {}); }
    const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const uuids::uuid & interfaceUUID) override
        { return resolveAll(interfaceUUID, {}); }

    SRef<IComponentIntrospect> resolve(const SPtr<InjectableMetadata> & injectableInfo,
                                       std::deque<BindContext> contextLevels);
    SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID,
                                       std::deque<BindContext> contextLevels);
    SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID, const std::string & name,
                                       std::deque<BindContext> contextLevels);
    const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const SPtr<InjectableMetadata> & injectableInfo,
                                                   std::deque<BindContext> contextLevels);
    const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const uuids::uuid & interfaceUUID,
                                       std::deque<BindContext> contextLevels);
    uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID) override;
    uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID, const std::string & name) override;

    SRef<IFactory> createNewFactoryContext(ContextMode ctxMode) override;

    // IComponentManager methods
    SRef<IComponentIntrospect> createComponent(const uuids::uuid & componentUUID) override;
    SRef<IComponentIntrospect> createComponent(const char * instanceName, const uuids::uuid & componentUUID) override;

    // IAliasManager methods
    bool aliasExists(Type type, const std::string & name) override
        { return m_aliasManager->aliasExists(type, name); }

    void declareAlias(Type type, const std::string & name, const uuids::uuid & uuid) override
        { return m_aliasManager->declareAlias(type, name, uuid); }

    const uuids::uuid & resolveComponentAlias(const std::string & name) override
        { return m_aliasManager->resolveComponentAlias(name); }

    const uuids::uuid & resolveInterfaceAlias(const std::string & name) override
         { return m_aliasManager->resolveInterfaceAlias(name); }

    const uuids::uuid & resolveModuleAlias(const std::string & name) override
        { return m_aliasManager->resolveModuleAlias(name); }

    // IRegistryManager methods

    XPCFErrorCode loadModules(const char* folderPathStr, bool bRecurse) override
        { return m_resolver->loadModules(folderPathStr, bRecurse); }

    XPCFErrorCode loadModuleMetadata(const char* moduleName,
                                     const char* moduleFilePath) override
        { return m_resolver->loadModuleMetadata(moduleName, moduleFilePath); }

    const IEnumerable<SPtr<ModuleMetadata>> & getModulesMetadata() const override
        { return m_resolver->getModulesMetadata(); }

    SPtr<ComponentMetadata> findComponentMetadata(const uuids::uuid & componentUUID) const override
        { return m_resolver->findComponentMetadata(componentUUID); }

    uuids::uuid getModuleUUID(const uuids::uuid & componentUUID) const override
        { return m_resolver->getModuleUUID(componentUUID); }

    SPtr<ModuleMetadata> findModuleMetadata(const uuids::uuid & moduleUUID) const override
        { return m_resolver->findModuleMetadata(moduleUUID); }

    const IEnumerable<SPtr<InterfaceMetadata>> & getInterfacesMetadata() const override
        { return m_resolver->getInterfacesMetadata(); }

    SPtr<InterfaceMetadata> findInterfaceMetadata(const uuids::uuid & interfaceUUID) const override
        { return m_resolver->findInterfaceMetadata(interfaceUUID); }

    void unloadComponent () override final;

private:
    template < typename I, typename C, BindingScope scope = BindingScope::Transient > void bindCore();
    void bindCore(const uuids::uuid & interfaceUUID, const FactoryBindInfos & bindInfos);
    void declareBindings(tinyxml2::XMLElement * xmlModuleElt);
    void declareInjects(tinyxml2::XMLElement * xmlModuleElt);
    void declareBind(tinyxml2::XMLElement * xmlBindElt);
    void declareSingleBind(const uuids::uuid & interfaceUUID, tinyxml2::XMLElement * xmlBindElt);
    void declareMultiBind(const uuids::uuid & interfaceUUID, tinyxml2::XMLElement * xmlBindElt);
    void declareInject(tinyxml2::XMLElement * xmlBindElt);
    void declareSpecificBind(tinyxml2::XMLElement * xmlBindElt, const uuids::uuid & targetComponentUUID);
    FactoryBindInfos getComponentBindingInfos(tinyxml2::XMLElement * xmlBindElt);
    SRef<IComponentIntrospect> create(const uuids::uuid& componentUUID);
    XPCFErrorCode loadLibrary(fs::path configurationFilePath);
    template <class T> XPCFErrorCode load(fs::path folderPath);
    SRef<IComponentIntrospect> resolveFromModule(const uuids::uuid & componentUUID);
    SRef<IComponentIntrospect> resolveComponent(const FactoryBindInfos & bindInfos,
                                                std::deque<BindContext> contextLevels);
    void inject(SRef<IInjectable> component,
                std::deque<BindContext> contextLevels={}) final;
    SPtr<ModuleMetadata> resolveMetadataFromComponentUUID(const uuids::uuid & componentUUID) {
        return m_resolver->findModuleMetadata(m_resolver->getModuleUUID(componentUUID));
    }
    FactoryBindInfos resolveBind(const uuids::uuid & interfaceUUID, std::deque<BindContext> contextLevels);
    FactoryBindInfos resolveBind(const uuids::uuid & interfaceUUID, const std::string & name,  std::deque<BindContext> contextLevels );
#ifdef XPCF_WITH_LOGS
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;
    std::stringstream log(const FactoryBindInfos & bindInfos);
#endif
    // The factory context contains every declared bind
    SRef<FactoryContext> m_context;

    // component Uuid resolves to IComponentIntrospect reference
    std::map<uuids::uuid,SRef<IComponentIntrospect> > m_singletonInstances;

    // [component Uuid, name] resolves to IComponentIntrospect reference
    std::map<std::pair<uuids::uuid,std::string>,SRef<IComponentIntrospect> > m_namedSingletonInstances;

    std::map<uuids::uuid, FactoryBindInfos> m_coreBindings;
    std::map<uuids::uuid,SRef<IComponentIntrospect> > m_coreInstances;
    // component UUID resolves to create function
    std::map<uuids::uuid, std::function<SRef<IComponentIntrospect>(void)>> m_coreFactoryMethods;

    SRef<AbstractRegistry> m_resolver;
    SRef<AbstractAliasManager> m_aliasManager;
    SRef<AbstractPropertyManager> m_propertyManager;
};

template <> struct ComponentTraits<Factory>
{
    static constexpr const char * UUID = "D6B80168-5E3D-4CB8-B7DF-1261FFE67E9E";
    static constexpr const char * NAME = "XPCF::Factory";
    static constexpr const char * DESCRIPTION = "XPCF::Factory";
};


}}} //namespace org::bcom::xpcf

#endif
