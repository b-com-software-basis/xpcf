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
#include "Collection.h"
#include "tinyxmlhelper.h"
/*#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>*/
#include <boost/filesystem.hpp>


#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API ComponentManager : public ComponentBase,
        public virtual IComponentManager {
public:
    static ComponentManager* instance();
    XPCFErrorCode load() override;
    XPCFErrorCode load(const char* libraryFilePath) override;
    XPCFErrorCode load(const char* folderPathStr, bool bRecurse) override;
    void clear() override;
    SRef<IComponentIntrospect> createComponent(const uuids::uuid& componentUUID) final;
    SRef<IComponentIntrospect> createComponent(const char * instanceName, const uuids::uuid& componentUUID) override;
    void unloadComponent () override final;
    void releaseComponent(uuids::uuid componentUUID);

    XPCFErrorCode addModuleMetadata(SPtr<ModuleMetadata> metadata);
    const IEnumerable<SPtr<ModuleMetadata>> & getModulesMetadata() const override;
    SPtr<ComponentMetadata> findComponentMetadata(const uuids::uuid &) const override;
    uuids::uuid getModuleUUID(uuids::uuid componentUUID) const override;
    SPtr<ModuleMetadata> findModuleMetadata(const uuids::uuid &) const override;

    XPCFErrorCode addInterfaceMetadata(SPtr<InterfaceMetadata> metadata);
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
    XPCFErrorCode loadLibrary(fs::path aPath);
    fs::path getConfigPath(uuids::uuid componentUUID) const;
    XPCFErrorCode declareInterface(SRef<ComponentMetadata> componentInfo, tinyxml2::XMLElement *interfaceElt);
    XPCFErrorCode declareComponent(SRef<ModuleMetadata> moduleInfo, tinyxml2::XMLElement *componentElt);
    XPCFErrorCode declareModule(tinyxml2::XMLElement * xmlModuleElt, fs::path configurationFilePath);
    SRef<IComponentIntrospect> create(const uuids::uuid& componentUUID);

    //boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;

    Collection<SPtr<ModuleMetadata>,std::vector> m_modulesVector;
    Collection<SPtr<InterfaceMetadata>,std::vector> m_interfacesVector;
    std::map<uuids::uuid, uuids::uuid> m_componentModuleUUIDMap;
    std::map<uuids::uuid, SPtr<InterfaceMetadata>> m_interfacesMap;
    std::map<uuids::uuid, SPtr<ModuleMetadata>> m_modulesMap;
    std::map<uuids::uuid, fs::path> m_moduleConfigMap;

    bool m_libraryLoaded;
};

template <> struct ComponentTraits<ComponentManager>
{
    static constexpr const char * UUID = "F905BCCD-9658-4871-87B8-B328C27675E0";
    static constexpr const char * DESCRIPTION = "XPCF::ComponentManager";
};

}}} //namespace org::bcom::xpcf

#endif
