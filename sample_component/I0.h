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

#ifndef I0_h
#define I0_h 1

#include "xpcf/api/IComponentIntrospect.h"
/**
 *  @ingroup interfaces
 */
/**
   * @class I0
   * This interface declares a set of methods.
   * It inherits from org::bcom::xpcf::IComponentIntrospect to provide introspection capabilities.
   *
   * @note The InterfaceTraits<I0> template struct defines the I0 UUID and description.
   * It allows I0 users to use directly the I0 type instead of its UUID to bind the component to this interface.
   */
class I0 : public virtual org::bcom::xpcf::IComponentIntrospect
{

public:
    virtual ~I0() = default;
    virtual void I0_opname () = 0;
    virtual void I0_opname2 () = 0;
    virtual void I0_opname3 () = 0;
    virtual void I0_opname4 () = 0;
    virtual void I0_opname5 () = 0;

};

/**
   * @struct InterfaceTraits<I0> instanciation.
   *
   * The code below shows the instanciation of the InterfaceTraits template struct for I0 interface type.
   * The syntax is the full template instanciation syntax.
   */
template <> struct org::bcom::xpcf::InterfaceTraits<::I0>
{
    static constexpr const char * UUID = "46333fd5-9eeb-4c9a-9912-b7df96ccf5fc";
    static constexpr const char * DESCRIPTION = "I0 interface";
};


#endif
