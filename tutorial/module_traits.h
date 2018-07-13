/**
 * @copyright Copyright (c) 2018 B-com http://www.b-com.com/
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

#ifndef MODULE_TRAITS_H
#define MODULE_TRAITS_H 1

#include "xpcf/component/ComponentTraits.h"

namespace client {
    class ComponentTemplate;
}

/**
   * @struct ComponentTraits<client::ComponentTemplate> instanciation
   * The code below shows the instanciation of the ComponentTraits template struct for client::ComponentTemplate type.
   * The syntax is the full template instanciation syntax.
   * It only requires "ComponentTraits.h" inclusion.
   */
   template <> struct org::bcom::xpcf::ComponentTraits<client::ComponentTemplate>
   {
        // a new UUID for each component must be created !
       static constexpr const char * UUID = "40F7D2F4-523E-4497-A848-9A6F17456019";
       static constexpr const char * DESCRIPTION = "client::ComponentTemplate - not configurable";
   };

#endif
