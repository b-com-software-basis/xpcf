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
 * @date 2017-03-07
 */

#ifndef IHUMAN_H
#define IHUMAN_H 1

#include "xpcf/api/IComponentIntrospect.h"
/**
 *  @ingroup interfaces
 */
/**
   * @class IHuman
   * This interface declares a set of methods describing a human being behaviors.
   * It inherits from org::bcom::xpcf::IComponentIntrospect to provide introspection capabilities.
   *
   * @note The InterfaceTraits<IHuman> template struct defines the IHuman UUID and description.
   * It allows IHuman users to use directly the IHuman type instead of its UUID to bind the component to this interface.
   */


class [[xpcf::clientUUID(2fff72e8-b192-477d-83d7-7dad07564c46)]] [[xpcf::serverUUID(c21d04f4-6f3e-4a79-b541-b094bb64cb95)]] IHuman :
    virtual public org::bcom::xpcf::IComponentIntrospect
{

public:
    virtual ~IHuman() = default;
    virtual void play () = 0;
    virtual void eat () = 0;
    virtual void sleep () = 0;
    virtual void kiss () = 0;
    virtual void work () = 0;

};

/**
   * @struct InterfaceTraits<IHuman> instanciation.
   *
   * The code below shows the instanciation of the InterfaceTraits template struct for IHuman interface type.
   * The syntax is the full template instanciation syntax.
   */
template <> struct org::bcom::xpcf::InterfaceTraits<::IHuman>
{
    static constexpr const char * UUID = "46333fd5-9eeb-4c9a-9912-b7df96ccf5fc";
    static constexpr const char * NAME = "IHuman";
    static constexpr const char * DESCRIPTION = "provides human related methods";
};


#endif
