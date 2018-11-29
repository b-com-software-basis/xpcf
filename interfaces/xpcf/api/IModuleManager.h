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

#ifndef ORG_BCOM_XPCF_IMODULEMANAGER_H
#define ORG_BCOM_XPCF_IMODULEMANAGER_H

#include "xpcf/api/IComponentIntrospect.h"
#include "xpcf/api/ModuleMetadata.h"

#include "xpcf/core/XPCFErrorCode.h"

namespace org { namespace bcom { namespace xpcf {

//TODO :
// - multi configuration files support : implemented TO TEST !!
// (now isLoaded is set once a first configuration file is successfully loaded,
// but doesn't prevent from loading other configuration files)
// - configuration file generation from : 1/ a module 2/ a path to search modules for
/**
 * @class IComponentManager
 * @brief Specifies the IComponentManager interface.
 *
 * This interface provides method definitions to handle component description file
 * loading and component creation.
 * The ComponentManaager is a singleton service.
 * Its instance can be retrieved with the dedicated getComponentManagerInstance() function.
 */
class IModuleManager : public virtual IComponentIntrospect {
public:
    /**
     * Virtual destructor of IComponentManager
     */
    virtual ~IModuleManager() = default;

    //virtual SPtr<tinyxml2::XMLDocument> introspectModule(const char* moduleFilePath) = 0;
    /**
     * Instropect the module to figure out the components contained in the module
     * @param [in] the module file path
     * @return
     */
    virtual SPtr<ModuleMetadata> introspectModule(const char* moduleName,
                                                  const char* moduleFilePath) = 0;

    virtual XPCFErrorCode saveModuleInformations(const char * xmlFilePath,
                                                 const SPtr<ModuleMetadata> & moduleInfos) = 0;

    /**
     * Create a component identified with @em componentUUID from the module information
     * provided in @em moduleInfos metadata.
     *
     * @param [in] moduleInfos : the module metadata information
     * @param [in] componentUUID : the component unique identifier
     * @note : the reference can be empty if no @em componentUUID exists in @em moduleInfos
     * @return the reference on the created component
     */
    virtual SRef<IComponentIntrospect> createComponent(SPtr<ModuleMetadata> moduleInfos,
                                       const uuids::uuid& componentUUID) = 0;

};

template <> struct InterfaceTraits<IModuleManager>
{
    static constexpr const char * UUID = "5FE6CCDF-2938-41C7-83F4-43C773AF0B05";
    static constexpr const char * NAME = "XPCF::IModuleManager";
    static constexpr const char * DESCRIPTION = "Provides module management functionality to introspect module content and component creation from an XPCF module";

};

/**
 * Retrieve the module manager instance.
 * @return the smart reference on the ModuleManager interface
 */
XPCF_EXPORT_API SRef<IModuleManager> getModuleManagerInstance();

}}} //namespace org::bcom::xpcf

#endif
