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
 * @date 2017-08-18
 */

#ifndef ORG_BCOM_XPCF_COMPONENTMANAGER_H
#define ORG_BCOM_XPCF_COMPONENTMANAGER_H

//#define BOOST_ALL_DYN_LINK 1

#include "xpcf/api/IComponentManager.h"
#include "xpcf/component/ComponentBase.h"
#include <xpcf/collection/Collection.h>
#include "tinyxmlhelper.h"

#ifdef XPCF_WITH_LOGS
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#endif

#include <boost/filesystem.hpp>
#include "Factory.h"
#include "Registry.h"
#include "AliasManager.h"

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API ComponentManager : public ComponentBase,
        virtual public IComponentManager {
public:
    static ComponentManager* instance();
    XPCFErrorCode load() override;
    XPCFErrorCode load(const char* libraryFilePath) override;
    XPCFErrorCode load(const char* folderPathStr, bool bRecurse) override;
    XPCFErrorCode loadModules(const char* folderPathStr, bool bRecurse) override;
    XPCFErrorCode loadModuleMetadata(const char* moduleName,
                                     const char* moduleFilePath) override;
    void clear() override;
    SRef<IComponentIntrospect> createComponent(const uuids::uuid & componentUUID) final;
    SRef<IComponentIntrospect> createComponent(const char * instanceName, const uuids::uuid& componentUUID) override;
    SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID)  override;
    SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID, const char * name) override;

    void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID, IComponentManager::Scope scope = IComponentManager::Scope::Transient) override;
    void bind(const char * name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID, IComponentManager::Scope scope = IComponentManager::Scope::Transient) override;
    void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
              const uuids::uuid & instanceUUID, IComponentManager::Scope scope = IComponentManager::Scope::Transient,
              IComponentManager::BindingRange range = IComponentManager::BindingRange::Explicit) override;
    void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
              const uuids::uuid & instanceUUID, IComponentManager::Scope scope = IComponentManager::Scope::Transient,
              IComponentManager::BindingRange range = IComponentManager::BindingRange::Explicit) override;
    void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
              const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
              IComponentManager::Scope scope = IComponentManager::Scope::Transient) override;
    void bind(const char * name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
              const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
              IComponentManager::Scope scope = IComponentManager::Scope::Transient) override;
    void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
              const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
              const uuids::uuid & instanceUUID, IComponentManager::Scope scope = IComponentManager::Scope::Transient,
              IComponentManager::BindingRange range = IComponentManager::BindingRange::Explicit) override;

    void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
              const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
              const uuids::uuid & instanceUUID, IComponentManager::Scope scope = IComponentManager::Scope::Transient,
              IComponentManager::BindingRange range = IComponentManager::BindingRange::Explicit) override;

    void unloadComponent () override final;
    void releaseComponent(uuids::uuid componentUUID);

    const IEnumerable<SPtr<ModuleMetadata>> & getModulesMetadata() const override;
    SPtr<ComponentMetadata> findComponentMetadata(const uuids::uuid &) const override;
    uuids::uuid getModuleUUID(const uuids::uuid & componentUUID) const override;
    SPtr<ModuleMetadata> findModuleMetadata(const uuids::uuid &) const override;

    const IEnumerable<SPtr<InterfaceMetadata>> & getInterfacesMetadata() const override;
    SPtr<InterfaceMetadata> findInterfaceMetadata(const uuids::uuid&) const override;

private:
    ComponentManager();
    ~ComponentManager() override = default;
    ComponentManager(const ComponentManager&)= delete;
    ComponentManager& operator=(const ComponentManager&)= delete;
    static std::atomic<ComponentManager*> m_instance;
    static std::mutex m_mutex;

    template <class T> XPCFErrorCode load(fs::path folderPath);
    template <class T> XPCFErrorCode loadModules(fs::path folderPath);
    XPCFErrorCode loadLibrary(fs::path aPath);
    SRef<IComponentIntrospect> create(const uuids::uuid& componentUUID);
    void inject(SRef<IInjectable> component);
#ifdef XPCF_WITH_LOGS
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;
#endif
    SRef<IFactory> m_factory;
    SRef<IRegistry> m_registry;
    SRef<IAliasManager> m_aliasManager;
    SRef<IPropertyManager> m_propertyManager;

    bool m_libraryLoaded;
};

template <> struct ComponentTraits<ComponentManager>
{
    static constexpr const char * UUID = "F905BCCD-9658-4871-87B8-B328C27675E0";
    static constexpr const char * NAME = "XPCF::ComponentManager";
    static constexpr const char * DESCRIPTION = "XPCF::ComponentManager";
};

}}} //namespace org::bcom::xpcf

#endif
