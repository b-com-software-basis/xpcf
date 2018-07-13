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

#ifndef I1_h
#define I1_h 1

#include "xpcf/api/IComponentIntrospect.h"
/**
 *  @ingroup interfaces
 */
/**
   * @class I1
   * This interface declares a set of methods.
   * It inherits from org::bcom::xpcf::IComponentIntrospect to provide introspection capabilities.
   *
   * @note The InterfaceTraits<I1> template struct defines the I1 UUID and description.
   * It allows I1 users to use directly the I0 type instead of its UUID to bind the component to this interface.
   */
class I1 : public virtual org::bcom::xpcf::IComponentIntrospect
{
public:
    virtual ~I1() = default;
    virtual void I1_opname () = 0;
    virtual void I1_opname2 () = 0;
    virtual void I1_opname3 () = 0;
    virtual void I1_opname4 () = 0;
    virtual void I1_opname5 () = 0;

};

/**
   * @struct  InterfaceTraits<I1> instanciation.
   *
   * The code below shows the instanciation of the InterfaceTraits template struct for I1 interface type.
   * The syntax uses the XPCF_DEFINE_INTERFACE_TRAITS helper macro.
   *
   * @note although the syntax seems easier, it obfuscates the InterfaceTraits<InterfaceType> expansion.
   */

XPCF_DEFINE_INTERFACE_TRAITS(::I1,
                             "3bc8f5ea-ee24-473e-8afd-4f5b1b21c018",
                             "I1 interface");

#endif
