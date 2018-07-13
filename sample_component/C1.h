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

#ifndef C1_H
#define C1_H 1

#include "I2.h"

#include "xpcfSampleComponent_traits.h"

#include "xpcf/component/ConfigurableBase.h"

#include <string>
/**
 *  @ingroup components
 */
namespace component {

/**
   * @class C1
   * This component shows the implementation of a configurable component.
   * It inherits the ConfigurableBase class, hence it provides IConfigurable capabilities.
   * C1 implementation wraps some of its inner members' data as IProperties, giving a generic and public access to those members.
   * C1 configuration is declared in the components' configuration file.
   *
   * @note When a C1 component is created through the XPCF ComponentManager it is also configured with the default values declared in the xpcf_registry_test.xml.
   * @note The needed ComponentTraits<C1> template struct defining the C1 UUID and description is located
   * in the xpcfSampleComponent_traits.h header file.
   * It allows C1 users to use directly a forward declared type
   * to invoke C1 creation without depending on the concrete implementation.
   */
class C1 : public org::bcom::xpcf::ConfigurableBase,
        public I2

{
public:
    C1 ();
    ~C1() override;
    void unloadComponent () override final;

    void I1_opname () override;
    void I1_opname2 () override;
    void I1_opname3 () override;
    void I1_opname4 () override;
    void I1_opname5 () override;

    void I2_opname () override;
    void I2_opname2 () override;

private:
    std::string m_name;
    // Component properties defined as members' data
    double m_blurFactor;
    std::vector<double> m_blurScales;
    int64_t m_cols;
    int64_t m_rows;
    std::vector<double> m_matrixValues;
    SRef<org::bcom::xpcf::IPropertyMap> m_blurMatrixRepartition;
};

}

#endif
