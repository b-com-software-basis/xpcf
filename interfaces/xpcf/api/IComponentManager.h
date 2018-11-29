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
 * @date 2017-04-28
 */

#ifndef ORG_BCOM_XPCF_ICOMPONENTMANAGER_H
#define ORG_BCOM_XPCF_ICOMPONENTMANAGER_H

#include "xpcf/api/IComponentIntrospect.h"
#include "xpcf/api/InterfaceMetadata.h"
#include "xpcf/api/ComponentMetadata.h"
#include "xpcf/api/ModuleMetadata.h"
#include "xpcf/core/XPCFErrorCode.h"

namespace org { namespace bcom { namespace xpcf {

/**
 * @class IComponentManager
 * @brief Specifies the IComponentManager interface.
 *
 * This interface provides method definitions to handle component description
 * file loading and component creation.
 * The ComponentManaager is a singleton service.
 * Its instance can be retrieved with the dedicated getComponentManagerInstance() function.
 */
class IComponentManager : public virtual IComponentIntrospect {
public:
    /**
     * Virtual destructor of IComponentManager
     */
    virtual ~IComponentManager() = default;

    /**
     * Clear all previously loaded registries.
     * @NOTE created components are not released, and their module counterparts are still loaded in memory.
     */
    virtual void clear() = 0;

    /**
     * Search the registry file and load it with all the components
     * @return
     */
    virtual XPCFErrorCode load() = 0;

    /**
     * Search the registry file and load it with all the components
     * @return
     */
    virtual XPCFErrorCode load(const char* libraryFilePath) = 0;

    /**
     * Find registry files from a root folder and load each registry file with its components
     * @param [in] folderPathStr : the root path to search xml registry's file for
     * @param [in] bRecurse : indicates to search recursively in subfolder [true] or only in @em folderPathStr [false]
     * @return
     */
    virtual XPCFErrorCode load(const char* folderPathStr, bool bRecurse) = 0;

    /**
     *
     * @note with @fn createComponent()
     * @param [in]
     * @return
     */
    template <class I> SRef<I> createComponent(const uuids::uuid& componentUUID);
    template <class I> SRef<I> createComponent(const char * instanceName, const uuids::uuid& componentUUID);

    template <class C> SRef<IComponentIntrospect> create();
    template <class C> SRef<IComponentIntrospect> create(const char * instanceName);

    /**
     *
     * @param [in] componentUUID
     * @return
     */
    virtual SRef<IComponentIntrospect> createComponent(const uuids::uuid& componentUUID) = 0;

    virtual SRef<IComponentIntrospect> createComponent(const char * instanceName, const uuids::uuid& componentUUID) = 0;

    /**
     *
     * @param [in]
     * @return
     */
    virtual const IEnumerable<SPtr<ModuleMetadata>> & getModulesMetadata() const = 0;

    /**
     *
     * @param [in]
     * @throws ComponentNotFoundException
     * @return
     */
    virtual SPtr<ComponentMetadata> findComponentMetadata(const uuids::uuid &) const = 0;

    /**
     *
     * @param [in]
     * @throws ModuleNotFoundException
     * @return
     */
    virtual uuids::uuid getModuleUUID(uuids::uuid componentUUID) const = 0;

    /**
     *
     * @param [in]
     * @throws ModuleNotFoundException
     * @return
     */
    virtual SPtr<ModuleMetadata> findModuleMetadata(const uuids::uuid &) const = 0;
    /**
     *
     * @param [in]
     * @return
     */
    virtual const IEnumerable<SPtr<InterfaceMetadata>> & getInterfacesMetadata() const = 0;

    /**
     * deprecatd ?? TODO : check
     * @param [in]
     * @return
     */
    [[deprecated]]
    virtual SPtr<InterfaceMetadata> findInterfaceMetadata(const uuids::uuid&) const = 0;

};

template <> struct InterfaceTraits<IComponentManager>
{
    static constexpr const char * UUID = "F3DBCB05-B8C6-47FB-BF80-E86D97DA46B8";
    static constexpr const char * NAME = "XPCF::IComponentManager";
    static constexpr const char * DESCRIPTION = "Provides component factory facility to create components from modules declared in a registry";
};

template <class I>
SRef<I> IComponentManager::createComponent(const uuids::uuid& componentUUID)
{
    SRef<IComponentIntrospect> rICIntrospect = createComponent(componentUUID);
    return rICIntrospect->bindTo<I>();
}

template <class C>
SRef<IComponentIntrospect> IComponentManager::create()
{
    return createComponent(toUUID<C>());
}

template <class I>
SRef<I> IComponentManager::createComponent(const char * instanceName, const uuids::uuid& componentUUID)
{
    SRef<IComponentIntrospect> rICIntrospect = createComponent(instanceName,componentUUID);
    return rICIntrospect->bindTo<I>();
}

template <class C>
SRef<IComponentIntrospect> IComponentManager::create(const char * instanceName)
{
    return createComponent(instanceName,toUUID<C>());
}

/**
 * Retrieve the component manager instance.
 * @return the smart reference on the ComponentManager interface
 */
XPCF_EXPORT_API SRef<IComponentManager> getComponentManagerInstance();

}}} //namespace org::bcom::xpcf

#endif
