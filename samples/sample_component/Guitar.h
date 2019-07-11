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

#ifndef GUITAR_H
#define GUITAR_H 1

#include "IGuitar.h"
#include "xpcf/component/ComponentBase.h"
#include "xpcf/component/ComponentTraits.h"

#include <string>
/**
 *  @ingroup components
 */
namespace component {

/**
   * @class Guitar
   * This component shows the implementation of a configurable component.
   * It inherits the ConfigurableBase class, hence it provides IConfigurable capabilities.
   * Guitar implementation wraps some of its inner members' data as IProperties, giving a generic and public access to those members.
   * Guitar configuration is declared in the components' configuration file.
   *
   * @note When a Guitar component is created through the XPCF ComponentManager it is also configured with the default values declared in the xpcf_registry_test.xml.
   * @note The needed ComponentTraits<Guitar> template struct defining the Guitar UUID and description is located
   * in the xpcfSampleComponent_traits.h header file.
   * It allows Guitar users to use directly a forward declared type
   * to invoke Guitar creation without depending on the concrete implementation.
   */
class Guitar : public org::bcom::xpcf::ComponentBase,
        public virtual IGuitar

{
public:
    Guitar(std::map<std::string,std::string> componentInfosMap);
    virtual ~Guitar() override;
    void unloadComponent () override final;
    const char *  getGuitarBrand () override { return m_brandName.c_str(); }
    uint32_t getNbStrings () override { return m_nbStrings; }
    GuitarType getGuitarType () override { return static_cast<IGuitar::GuitarType>(m_type); }

protected:
    // Component properties defined as members' data
    std::string m_brandName;
    uint32_t m_nbStrings;
    uint32_t m_type;
};

}


#endif
