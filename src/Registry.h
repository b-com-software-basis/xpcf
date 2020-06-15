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

#ifndef ORG_BCOM_XPCF_REGISTRY_H
#define ORG_BCOM_XPCF_REGISTRY_H

//#define BOOST_ALL_DYN_LINK 1

#include <xpcf/collection/Collection.h>
#include "tinyxmlhelper.h"
#include <xpcf/api/InterfaceMetadata.h>
#include <xpcf/api/ComponentMetadata.h>
#include <xpcf/api/ModuleMetadata.h>
#include <xpcf/api/IComponentIntrospect.h>
#include <xpcf/component/ComponentBase.h>
#include "AliasManager.h"

#ifdef XPCF_WITH_LOGS
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#endif

#include <boost/filesystem.hpp>

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

namespace org { namespace bcom { namespace xpcf {

class IRegistry  : virtual public IComponentIntrospect {
public:
    ~IRegistry() override  = default;
    virtual void clear() = 0;

    virtual void setBinder(const std::function<void(const uuids::uuid &, const uuids::uuid &)> & bindFunc) = 0;

    virtual void addModuleMetadata(SPtr<ModuleMetadata> metadata) = 0;
    virtual void declareModuleMetadata(SPtr<ModuleMetadata> moduleInfos) = 0;
    virtual const IEnumerable<SPtr<ModuleMetadata>> & getModulesMetadata() const = 0;
    virtual SPtr<ComponentMetadata> findComponentMetadata(const uuids::uuid & componentUUID) const = 0;
    virtual uuids::uuid getModuleUUID(const uuids::uuid & componentUUID) const = 0;
    virtual SPtr<ModuleMetadata> findModuleMetadata(const uuids::uuid & moduleUUID) const = 0;

    virtual void addInterfaceMetadata(SPtr<InterfaceMetadata> metadata) = 0;
    virtual const IEnumerable<SPtr<InterfaceMetadata>> & getInterfacesMetadata() const = 0;
    virtual SPtr<InterfaceMetadata> findInterfaceMetadata(const uuids::uuid & interfaceUUID) const = 0;
    virtual void declareModule(tinyxml2::XMLElement * xmlModuleElt) = 0;
    virtual void enableAutoAlias(bool enabled) = 0;
};

template <> struct InterfaceTraits<IRegistry>
{
    static constexpr const char * UUID = "3AE3A5D2-F200-4078-89EF-5AE1F5EF1A17";
    static constexpr const char * NAME = "XPCF::IRegistry";
    static constexpr const char * DESCRIPTION = "Registry interface.\nProvides components/modules registry";
};

class Registry : public ComponentBase,
        virtual public IRegistry {
public:
    Registry();
    //Registry(const std::function<void(const uuids::uuid &, const uuids::uuid &)> & bindFunc):m_autobind(bindFunc) {}
    ~Registry() override = default;
    void clear() override;
    void setBinder(const std::function<void(const uuids::uuid &, const uuids::uuid &)> & bindFunc) override { m_autobind = bindFunc; }
    void addModuleMetadata(SPtr<ModuleMetadata> metadata) override;
    void declareModuleMetadata(SPtr<ModuleMetadata> moduleInfos) override;
    const IEnumerable<SPtr<ModuleMetadata>> & getModulesMetadata() const  override;
    SPtr<ComponentMetadata> findComponentMetadata(const uuids::uuid & componentUUID) const  override;
    uuids::uuid getModuleUUID(const uuids::uuid & componentUUID) const  override;
    SPtr<ModuleMetadata> findModuleMetadata(const uuids::uuid & moduleUUID) const  override;

    void addInterfaceMetadata(SPtr<InterfaceMetadata> metadata) override;
    const IEnumerable<SPtr<InterfaceMetadata>> & getInterfacesMetadata() const  override;
    SPtr<InterfaceMetadata> findInterfaceMetadata(const uuids::uuid & interfaceUUID) const  override;
    void declareModule(tinyxml2::XMLElement * xmlModuleElt) override;
    void enableAutoAlias(bool enabled) override { m_autoAlias = enabled; }
    void unloadComponent () override final;

private:
    void declareInterfaceNode(SRef<ComponentMetadata> componentInfo, tinyxml2::XMLElement *interfaceElt);
    void declareComponent(SRef<ModuleMetadata> moduleInfo, tinyxml2::XMLElement *componentElt);
    void autobind(const uuids::uuid & interfaceUUID, const uuids::uuid & componentUUID) { m_autobind(interfaceUUID,componentUUID); }

#ifdef XPCF_WITH_LOGS
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;
#endif

    Collection<SPtr<ModuleMetadata>,std::vector> m_modulesVector;
    Collection<SPtr<InterfaceMetadata>,std::vector> m_interfacesVector;
    std::map<uuids::uuid, uuids::uuid> m_componentModuleUUIDMap;
    std::map<uuids::uuid, SPtr<InterfaceMetadata>> m_interfacesMap;
    std::map<uuids::uuid, SPtr<ModuleMetadata>> m_modulesMap;
    std::function<void(const uuids::uuid &, const uuids::uuid &)> m_autobind = [](const uuids::uuid &, const uuids::uuid &) -> void {};
    SRef<IAliasManager> m_aliasManager;
    bool m_libraryLoaded;
    bool m_autoAlias = false;
};


template <> struct ComponentTraits<Registry>
{
    static constexpr const char * UUID = "C79C683D-64F4-4F7F-B43A-EF8F368F9940";
    static constexpr const char * NAME = "XPCF::Registry";
    static constexpr const char * DESCRIPTION = "XPCF::Registry";
};


}}} //namespace org::bcom::xpcf

#endif
