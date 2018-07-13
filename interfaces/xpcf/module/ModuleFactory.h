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

#ifndef ORG_BCOM_XPCF_MODULEFACTORY_H
#define ORG_BCOM_XPCF_MODULEFACTORY_H

#include "xpcf/core/XPCFErrorCode.h"
#include "xpcf/component/ComponentFactory.h"
#include "xpcf/module/IModuleIndex.h"

#include <string>

namespace org { namespace bcom { namespace xpcf {

#define XPCF_GETCOMPONENT "XPCF_getComponent"
#define XPCF_GETMODULEUUID "XPCF_getModuleUUID"
#define XPCF_GETMODULENAME "XPCF_getModuleName"
#define XPCF_GETMODULEINDEX "XPCF_getModuleIndex"

// getComponent signature is :
// XPCFErrorCode XPCF_getComponent(const uuids::uuid &, SRef<IComponentIntrospect>&)

#define EXPORT_XPCF_getModuleUUID_FUNCTION \
extern "C" XPCF_MODULEHOOKS_API const char * XPCF_getModuleUUID() \
{ \
    return moduleUUID.c_str(); \
}

#define EXPORT_XPCF_getModuleName_FUNCTION \
extern "C" XPCF_MODULEHOOKS_API const char * XPCF_getModuleName() \
{ \
    return moduleName.c_str(); \
}


#define XPCF_BEGIN_COMPONENTS_DECLARATION \
extern "C" XPCF_MODULEHOOKS_API \
void XPCF_getModuleIndex(SRef<org::bcom::xpcf::IModuleIndex> moduleIndexRef) {

#define XPCF_ADD_COMPONENT(fullComponentType) \
moduleIndexRef->declareComponent<fullComponentType>();

#define XPCF_END_COMPONENTS_DECLARATION }

#define XPCF_DECLARE_MODULE(UUID, name) static std::string moduleUUID = UUID; \
    static std::string moduleName = name; \
    EXPORT_XPCF_getModuleUUID_FUNCTION \
    EXPORT_XPCF_getModuleName_FUNCTION


template <class C> XPCFErrorCode tryCreateComponent(const uuids::uuid& expectedComponentUUID,
                                                    SRef<IComponentIntrospect>& interfaceRef)
{
    XPCFErrorCode result = XPCFErrorCode::_FAIL;
    uuids::uuid declaredComponentUUID = toUUID<C>();
    if (expectedComponentUUID == declaredComponentUUID)
    {
        interfaceRef = ComponentFactory::create<C>();
        if (interfaceRef) {
            result = XPCFErrorCode::_SUCCESS;
        }
    }
    return result;
}


}}} //namespace org::bcom::xpcf

#endif
