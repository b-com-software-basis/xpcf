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
 * @date 2017-11-22
 */


#ifndef ORG_BCOM_XPCF_MODULEMANAGER_H
#define ORG_BCOM_XPCF_MODULEMANAGER_H

#include "xpcf/api/IModuleManager.h"
#include "xpcf/component/ComponentBase.h"
#include "xpcf/api/ComponentMetadata.h"
#include <vector>
#include <atomic>
#include <mutex>

namespace org { namespace bcom { namespace xpcf {

class ModuleManager : public ComponentBase,
        public virtual IModuleManager {
public:
    static ModuleManager* instance();
    bool isXpcfModule(const char* modulePath) override;
    bool isXpcfModule(const char* moduleName, const char* moduleFolderPath) override;
    bool isXpcfModule(SPtr<ModuleMetadata> moduleInfos)  override;
    const char * getXpcfVersion(const char* moduleName, const char* moduleFolderPath) override;
    SPtr<ModuleMetadata> introspectModule(const char* moduleFilePath) override;
    SPtr<ModuleMetadata> introspectModule(const char* moduleName, const char* moduleFolderPath) override;
    XPCFErrorCode saveModuleInformations(const char * xmlFilePath,
                                         const SPtr<ModuleMetadata> & moduleInfos) override;
    SRef<IComponentIntrospect> createComponent(SPtr<ModuleMetadata> moduleInfos, const uuids::uuid& componentUUID) override;

    void unloadComponent () override final;
    void releaseModuleRef(const uuids::uuid& moduleUUID);

private:
    ModuleManager();
    ~ModuleManager() override = default;
    ModuleManager(const ModuleManager&)= delete;
    ModuleManager& operator=(const ModuleManager&)= delete;
    void addModuleRef(const uuids::uuid& moduleUUID);
   // void releaseModuleRef(const uuids::uuid& moduleUUID);
    bool isXpcfModule(fs::path modulePath);
    SPtr<ModuleMetadata> introspectModule(fs::path modulePath);
    std::vector<SRef<InterfaceMetadata>> getComponentInterfaceList(SPtr<ModuleMetadata> moduleInfos,
                                                                   const uuids::uuid& componentUUID);
    static std::atomic<ModuleManager*> m_instance;
    static std::mutex m_mutex;
    //boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;

    std::vector<SPtr<ComponentMetadata>> m_componentsVector;
    std::vector<SPtr<InterfaceMetadata>> m_interfacesVector;
    std::map<uuids::uuid, SPtr<ComponentMetadata>> m_componentsMap;
    std::map<uuids::uuid, SPtr<InterfaceMetadata>> m_interfacesMap;
    std::map<uuids::uuid, SPtr<ModuleMetadata>> m_moduleMap;
    std::map<uuids::uuid, boost::function<XPCFErrorCode(const uuids::uuid &, SRef<IComponentIntrospect>&)>> m_funcMap;
};

template <> struct ComponentTraits<ModuleManager>
{
    static constexpr const char * UUID = "AF5F7F91-492D-4693-929F-4852C721AC63";
    static constexpr const char * DESCRIPTION = "XPCF::ModuleManager";
};

}}}

#endif // MODULEMANAGER_H
