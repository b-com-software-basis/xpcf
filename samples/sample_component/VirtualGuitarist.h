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

#ifndef VIRTUALGUITARIST_H
#define VIRTUALGUITARIST_H 1

#include "IGuitarist.h"

#include "xpcfSampleComponent_traits.h"
#include <xpcf/collection/ICollection.h>
#include <xpcf/component/ConfigurableBase.h>

#include <string>
/**
 *  @ingroup components
 */
namespace component {

/**
   * @class VirtualGuitarist
   * This component shows the implementation of a configurable component.
   * It inherits the ConfigurableBase class, hence it provides IConfigurable capabilities.
   * VirtualGuitarist implementation wraps some of its inner members' data as IProperties, giving a generic and public access to those members.
   * VirtualGuitarist configuration is declared in the components' configuration file.
   *
   * @note When a VirtualGuitarist component is created through the XPCF ComponentManager it is also configured with the default values declared in the xpcf_registry_test.xml.
   * @note The needed ComponentTraits<VirtualGuitarist> template struct defining the VirtualGuitarist UUID and description is located
   * in the xpcfSampleComponent_traits.h header file.
   * It allows VirtualGuitarist users to use directly a forward declared type
   * to invoke VirtualGuitarist creation without depending on the concrete implementation.
   */
class VirtualGuitarist : public org::bcom::xpcf::ConfigurableBase,
        public IGuitarist

{
public:
    VirtualGuitarist ();
    ~VirtualGuitarist() override;
    void unloadComponent () override final;

    void learn () override;
    void playMusic (const MusicScore & score) override;
    void playScore (std::vector<std::pair<std::string, float>> & score) override;
    void playModifyScore (std::vector<std::pair<std::string, float>> & score) override;
    void listen () override;
    void practice () override;
    void party () override;
    SRef<IGuitar> getGuitar() override { return m_guitar; }
    SRef<IGuitar> getGuitar(IGuitar::GuitarType type) override;
    std::vector<std::string> getGuitarCollection() override;

    void playSolo (MusicScore & score) override;
    void playRhythm () override;

    void onInjected() override;

private:
    SRef<IGuitar> m_guitar;
    SRef<IGuitar> m_folkGuitar;
    SRef<IElectricGuitar> m_electricGuitar;
    SRef<org::bcom::xpcf::ICollection<SRef<IElectricGuitar> >> m_guitars;
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
