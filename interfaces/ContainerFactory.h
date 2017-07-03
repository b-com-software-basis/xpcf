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

#ifndef __ContainerFactory_H__
#define __ContainerFactory_H__

#include <string>
#include <map>
#include <vector>

namespace org {
namespace bcom {
namespace xpcf {
#define XPCF_GETCOMPONENT "XPCF_getComponent"
#define XPCF_GETNBCOMPONENTS "XPCF_getNbComponents"
#define XPCF_GETCOMPONENTUUID "XPCF_getComponentUUID"
#define XPCF_GETCOMPONENTNAME "XPCF_getComponentName"
#define XPCF_GETCONTAINERUUID "XPCF_getContainerUUID"
#define XPCF_GETCONTAINERNAME "XPCF_getContainerName"

// getComponent signature is : long XPCF_getComponent(const uuids::uuid &, SRef<IComponentIntrospect>&)

#define EXPORT_XPCF_getNbComponents_FUNCTION	extern "C" XPCF_EXPORT_API  unsigned long XPCF_getNbComponents() \
{ \
    if (containerComponentUUIDArray.empty()) { \
        for (auto componentItem : containerComponentMap) { \
            containerComponentUUIDArray.push_back(componentItem.first); \
        } \
    } \
    return containerComponentUUIDArray.size(); \
}

#define EXPORT_XPCF_getComponentUUID_FUNCTION	extern "C" XPCF_EXPORT_API  const char * XPCF_getComponentUUID(unsigned long index) \
{ \
    const char * componentUUID = nullptr; \
    if (index < containerComponentUUIDArray.size()) { \
        componentUUID = containerComponentUUIDArray[index].c_str(); \
    } \
    return componentUUID; \
}


#define EXPORT_XPCF_getComponentName_FUNCTION extern "C" XPCF_EXPORT_API  const char * XPCF_getComponentName(const char * componentUUID) \
{ \
    std::string componentStrUUID = componentUUID; \
    if (containerComponentMap.find(componentStrUUID) != containerComponentMap.end()) { \
        return containerComponentMap.at(componentStrUUID).c_str(); \
    } \
    return nullptr; \
}

#define EXPORT_XPCF_getContainerUUID_FUNCTION	extern "C" XPCF_EXPORT_API  const char * XPCF_getContainerUUID() \
{ \
    return containerUUID.c_str(); \
}


#define EXPORT_XPCF_getContainerName_FUNCTION extern "C" XPCF_EXPORT_API  const char * XPCF_getContainerName() \
{ \
    return containerName.c_str(); \
}

#define XPCF_BEGIN_COMPONENTS_DECLARATION  static std::map<std::string,std::string> containerComponentMap = {

#define XPCF_ADD_COMPONENT(componentID,componentName) {componentID,componentName},

#define XPCF_END_COMPONENTS_DECLARATION }; \
static std::vector<std::string> containerComponentUUIDArray; \
EXPORT_XPCF_getNbComponents_FUNCTION \
EXPORT_XPCF_getComponentUUID_FUNCTION \
EXPORT_XPCF_getComponentName_FUNCTION

#define XPCF_DECLARE_CONTAINER(UUID,name) static std::string containerUUID = UUID; \
static std::string containerName = name; \
EXPORT_XPCF_getContainerUUID_FUNCTION \
EXPORT_XPCF_getContainerName_FUNCTION

}}} //namespace org::bcom::xpcf

#endif
