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

#ifndef XPCFSAMPLECOMPONENT_TRAITS_H
#define XPCFSAMPLECOMPONENT_TRAITS_H 1

#include "xpcf/component/ComponentTraits.h"
/** @defgroup sampleComponent Sample Component
 *  This is the sample component group
 */
/** @defgroup interfaces Interfaces
 *  @ingroup sampleComponent
 *  This is the sample component interfaces group
 */
/** @defgroup xpcfmodule Module declaration
 *  @ingroup sampleComponent
 *  This is the sample component module declaration group
 */
/** @defgroup components Components
 *  @ingroup sampleComponent
 *  This is the sample component components group
 * @{
 */

namespace component {
    class HumanMusician;
    class VirtualGuitarist;
    class Ibanez7ElectricGuitar;
    class JacksonDinky6ElectricGuitar;
    class TakamineFolkGuitar;
}

/**
   * @struct ComponentTraits<HumanMusician> instanciation
   * The code below shows the instanciation of the ComponentTraits template struct for HumanMusician type.
   * The syntax is the full template instanciation syntax.
   * It only requires "ComponentTraits.h" inclusion.
   */
template <> struct org::bcom::xpcf::ComponentTraits<component::HumanMusician>
{
    static constexpr const char * UUID = "ae9d10bb-4bd1-462b-8c01-43f1815c6ae0";
    static constexpr const char * NAME = "HumanMusician component - not configurable";
    static constexpr const char * DESCRIPTION = "HumanMusician is not Configurable, it implements I0 and I1 interfaces";
};


/**
   * @struct ComponentTraits<VirtualGuitarist> instanciation
   * The code below shows the instanciation of the ComponentTraits template struct for VirtualGuitarist type.
   * The syntax uses the XPCF_DEFINE_COMPONENT_TRAITS helper macro.
   * It requires "traits.h" inclusion.
   *
   * @note although the syntax seems easier, it obfuscates the ComponentTraits<ComponentType> expansion and
   * thus can lead to misunderstanding upon later compilation issues (typically when the XXX_traits.h header inclusion is forgotten).
   */
#include "xpcf/core/traits.h"
XPCF_DEFINE_COMPONENT_TRAITS(component::VirtualGuitarist,
                             "{63FF193D-93E6-4EDE-9947-22F864AC843F}",
                             "VirtualGuitarist component",
                             "VirtualGuitarist implements IConfigurable, I2 and I1 interfaces");


/**
   * @struct ComponentTraits<Ibanez7ElectricGuitar> instanciation
   * The code below shows the instanciation of the ComponentTraits template struct for Ibanez7ElectricGuitar type.
   * The syntax is the full template instanciation syntax.
   * It only requires "ComponentTraits.h" inclusion.
   */
template <> struct org::bcom::xpcf::ComponentTraits<component::Ibanez7ElectricGuitar>
{
    static constexpr const char * UUID = "ED894181-0FDC-4326-A068-CB2A5899CB13";
    static constexpr const char * NAME = "Ibanez7ElectricGuitar component - with specific configuration";
    static constexpr const char * DESCRIPTION = "Ibanez7ElectricGuitar is not Configurable, it implements IElectricGuitar interface";
};

/**
   * @struct ComponentTraits<JacksonDinky6ElectricGuitar> instanciation
   * The code below shows the instanciation of the ComponentTraits template struct for JacksonDinky6ElectricGuitar type.
   * The syntax is the full template instanciation syntax.
   * It only requires "ComponentTraits.h" inclusion.
   */
template <> struct org::bcom::xpcf::ComponentTraits<component::JacksonDinky6ElectricGuitar>
{
    static constexpr const char * UUID = "7CC64F89-9A81-4FC8-9695-FDE123FAE225";
    static constexpr const char * NAME = "JacksonDinky6ElectricGuitar component - with specific configuration";
    static constexpr const char * DESCRIPTION = "JacksonDinky6ElectricGuitar is not Configurable, it implements IElectricGuitar interface";
};


/**
   * @struct ComponentTraits<TakamineFolkGuitar> instanciation
   * The code below shows the instanciation of the ComponentTraits template struct for TakamineFolkGuitar type.
   * The syntax is the full template instanciation syntax.
   * It only requires "ComponentTraits.h" inclusion.
   */
template <> struct org::bcom::xpcf::ComponentTraits<component::TakamineFolkGuitar>
{
    static constexpr const char * UUID = "{04E2254F-3349-4DC1-8EED-86835315FB6B}";
    static constexpr const char * NAME = "TakamineFolkGuitar component - with specific configuration";
    static constexpr const char * DESCRIPTION = "TakamineFolkGuitar is not Configurable, it implements IGuitar interface";
};


/** @}*/
#endif
