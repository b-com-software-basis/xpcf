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

#ifndef C0_H
#define C0_H 1

// I0
#include "I0.h"
// I1
#include "I1.h"

#include "xpcfSampleComponent_traits.h"

#include "xpcf/component/ComponentBase.h"

#include <string>
/**
 *  @ingroup components
 */
namespace component {


/**
   * @class C0
   * This component shows the implementation of a simple component.
   * It inherits the ComponentBase class, hence it does not provide IConfigurable capabilities (no property handling).
   * ComponentBase provides the minimum implementation for interface introspection.
   *
   * @note The needed ComponentTraits<C0> template struct defining the C0 UUID and description is located
   * in the xpcfSampleComponent_traits.h header file.
   * It allows C0 users to use directly a forward declared type
   * to invoke C0 creation without depending on the concrete implementation.
   */
class C0 : public org::bcom::xpcf::ComponentBase,
        public I0,
        public I1

{
public:
    C0 ();
    ~C0() override;
    void unloadComponent () override final;
    void I0_opname () override;
    void I0_opname2 () override;
    void I0_opname3 () override;
    void I0_opname4 () override;
    void I0_opname5 () override;

    void I1_opname () override;
    void I1_opname2 () override;
    void I1_opname3 () override;
    void I1_opname4 () override;
    void I1_opname5 () override;

private:
    std::string m_name;
};

}
#endif
