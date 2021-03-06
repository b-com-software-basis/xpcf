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
 * @date 2018-06-25
 */

#include "HumanMusician.h"
#include "VirtualGuitarist.h"
#include "Ibanez7ElectricGuitar.h"
#include "JacksonDinky6ElectricGuitar.h"
#include "TakamineFolkGuitar.h"
#include "Tuner.h"

#include "xpcf/module/ModuleFactory.h"
#include <iostream>

namespace xpcf=org::bcom::xpcf;

/**
 *  @ingroup xpcfmodule
 */
/**
  * Declare module.
  */
XPCF_DECLARE_MODULE("3b899ff0-e098-4218-bdaa-82abdec22960","xpcfSampleComponent","XPCF component example");

/**
 * This method is the module entry point.
 * XPCF uses this method to create components available in the module.
 *
 * Each component exposed must be declared inside a xpcf::tryCreateComponent<ComponentType>() call.
 */
extern "C" XPCF_MODULEHOOKS_API xpcf::XPCFErrorCode XPCF_getComponent(const xpcf::uuids::uuid& componentUUID,SRef<xpcf::IComponentIntrospect>& interfaceRef)
{
    xpcf::XPCFErrorCode errCode = xpcf::XPCFErrorCode::_FAIL;
    errCode = xpcf::tryCreateComponent<component::HumanMusician>(componentUUID,interfaceRef);
    if (errCode != xpcf::XPCFErrorCode::_SUCCESS) {
        errCode = xpcf::tryCreateComponent<component::VirtualGuitarist>(componentUUID,interfaceRef);
    }
    if (errCode != xpcf::XPCFErrorCode::_SUCCESS) {
        errCode = xpcf::tryCreateComponent<component::Ibanez7ElectricGuitar>(componentUUID,interfaceRef);
    }
    if (errCode != xpcf::XPCFErrorCode::_SUCCESS) {
        errCode = xpcf::tryCreateComponent<component::JacksonDinky6ElectricGuitar>(componentUUID,interfaceRef);
    }
    if (errCode != xpcf::XPCFErrorCode::_SUCCESS) {
        errCode = xpcf::tryCreateComponent<component::TakamineFolkGuitar>(componentUUID,interfaceRef);
    }
    if (errCode != xpcf::XPCFErrorCode::_SUCCESS) {
        errCode = xpcf::tryCreateComponent<component::Tuner>(componentUUID,interfaceRef);
    }
    return errCode;
}

/**
  * The declarations below populate list of the components available in the module (it represents the module index).
  * XPCF uses this index to introspect the components available in a module, providing the ability to generate the configuration file skeleton from the code.
  */
XPCF_BEGIN_COMPONENTS_DECLARATION
XPCF_ADD_COMPONENT(component::HumanMusician)
XPCF_ADD_COMPONENT(component::VirtualGuitarist)
XPCF_ADD_COMPONENT(component::Ibanez7ElectricGuitar)
XPCF_ADD_COMPONENT(component::JacksonDinky6ElectricGuitar)
XPCF_ADD_COMPONENT(component::TakamineFolkGuitar)
XPCF_ADD_COMPONENT(component::Tuner)
XPCF_END_COMPONENTS_DECLARATION
