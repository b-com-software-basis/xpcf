/**
 * @copyright Copyright (c) 2021 B-com http://www.b-com.com/
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
 * @date 2021-09-14
 */

#ifndef IREGISTRYMANAGER_H
#define IREGISTRYMANAGER_H

#include <xpcf/api/IComponentIntrospect.h>
#include <xpcf/api/InterfaceMetadata.h>
#include <xpcf/api/ComponentMetadata.h>
#include <xpcf/api/ModuleMetadata.h>
#include <xpcf/core/XPCFErrorCode.h>

namespace org { namespace bcom { namespace xpcf {

class IRegistryManager  : virtual public IComponentIntrospect {
public:
    ~IRegistryManager() override  = default;

    /**
     * Read a module metadata and add the metadata to the componentmanager registry
     * @param [in] the module name
     * @param [in] the module file path
     * @return
     */
    virtual XPCFErrorCode loadModuleMetadata(const char* moduleName,
                                             const char* moduleFilePath) = 0;

    /**
     * Find module files from a root folder and load each module metadata in XPCF registry
     * @param [in] folderPathStr : the root path to search module files' for
     * @param [in] bRecurse : indicates to search recursively in subfolder [true] or only in @p folderPathStr [false]
     * @return
     */
    virtual XPCFErrorCode loadModules(const char* folderPathStr, bool bRecurse = false) = 0;

    /**
     * Retrieve every module metadata registered in the ComponentManager
     * @return the collection of modules metadata
     */
    virtual const IEnumerable<SPtr<ModuleMetadata>> & getModulesMetadata() const = 0;

    /**
     * Search a component metadata.
     * @param [in] the component UUID to search for
     * @throws ComponentNotFoundException
     * @return the component metadata for the component UUID provided
     */
    virtual SPtr<ComponentMetadata> findComponentMetadata(const uuids::uuid & componentUUID) const = 0;

    /**
     * Search the module containing the component provided.
     * @param [in] the component UUID
     * @throws ComponentNotFoundException
     * @return the module UUID containing the component provided
     */
    virtual uuids::uuid getModuleUUID(const uuids::uuid & componentUUID) const = 0;

    /**
     * Search a module metadata.
     * @param [in] the module UUID to search for
     * @throws ModuleNotFoundException
     * @return the module metadata for the module UUID provided
     */
    virtual SPtr<ModuleMetadata> findModuleMetadata(const uuids::uuid & moduleUUID) const = 0;

    /**
     * Retrieve every interface metadata registered in the ComponentManager
     * @return the collection of interfaces metadata
     */
    virtual const IEnumerable<SPtr<InterfaceMetadata>> & getInterfacesMetadata() const = 0;

    /**
     * deprecatd ?? TODO : check
     * @param [in]
     * @return
     */
    virtual SPtr<InterfaceMetadata> findInterfaceMetadata(const uuids::uuid & interfaceUUID) const = 0;

    template <typename I> SPtr<InterfaceMetadata> findInterfaceMetadata();
    template <typename I> SPtr<ComponentMetadata> findComponentMetadata();
};

template <typename I> SPtr<InterfaceMetadata> IRegistryManager::findInterfaceMetadata()
{
    return findInterfaceMetadata( toUUID<I>() );
}

template <typename C> SPtr<ComponentMetadata> IRegistryManager::findComponentMetadata()
{
    return findComponentMetadata( toUUID<C>() );
}


template <> struct InterfaceTraits<IRegistryManager>
{
    static constexpr const char * UUID = "3AE3A5D2-F200-4078-89EF-5AE1F5EF1A17";
    static constexpr const char * NAME = "XPCF::IRegistryManager";
    static constexpr const char * DESCRIPTION = "RegistryManager interface.\nProvides components/modules registry";
};

}}}


#endif // IREGISTRYMANAGER_H
