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

#ifndef JACKSONDINKY6ELECTRICGUITAR_H
#define JACKSONDINKY6ELECTRICGUITAR_H 1

#include "Guitar.h"
#include "xpcfSampleComponent_traits.h"

#include <string>
/**
 *  @ingroup components
 */
namespace component {

/**
   * @class JacksonDinky6ElectricGuitar
   * This component shows the implementation of a configurable component.
   * It inherits the ConfigurableBase class, hence it provides IConfigurable capabilities.
   * JacksonDinky6ElectricGuitar implementation wraps some of its inner members' data as IProperties, giving a generic and public access to those members.
   * JacksonDinky6ElectricGuitar configuration is declared in the components' configuration file.
   *
   * @note When a JacksonDinky6ElectricGuitar component is created through the XPCF ComponentManager it is also configured with the default values declared in the xpcf_registry_test.xml.
   * @note The needed ComponentTraits<JacksonDinky6ElectricGuitar> template struct defining the JacksonDinky6ElectricGuitar UUID and description is located
   * in the xpcfSampleComponent_traits.h header file.
   * It allows JacksonDinky6ElectricGuitar users to use directly a forward declared type
   * to invoke JacksonDinky6ElectricGuitar creation without depending on the concrete implementation.
   */
class JacksonDinky6ElectricGuitar : public Guitar,  virtual public IElectricGuitar

{
public:
    JacksonDinky6ElectricGuitar();
    ~JacksonDinky6ElectricGuitar() override;
    void setDistortionLevel (uint32_t level) override { m_distortionLevel = level; }
    uint32_t getDistortionLevel () override { return m_distortionLevel; }

private:
    uint32_t m_distortionLevel;
};

}

#endif
