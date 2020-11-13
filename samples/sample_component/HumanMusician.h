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

#ifndef HUMANMUSICIAN_H
#define HUMANMUSICIAN_H 1

#include "IHuman.h"
#include "IMusician.h"

#include "xpcfSampleComponent_traits.h"

#include "xpcf/component/ComponentBase.h"

#include <string>
/**
 *  @ingroup components
 */
namespace component {


/**
   * @class HumanMusician
   * This component shows the implementation of a simple component.
   * It inherits the ComponentBase class, hence it does not provide IConfigurable capabilities (no property handling).
   * ComponentBase provides the minimum implementation for interface introspection.
   *
   * @note The needed ComponentTraits<HumanMusician> template struct defining the HumanMusician UUID and description is located
   * in the xpcfSampleComponent_traits.h header file.
   * It allows HumanMusician users to use directly a forward declared type
   * to invoke HumanMusician creation without depending on the concrete implementation.
   */
class HumanMusician : public org::bcom::xpcf::ComponentBase,
        public IHuman,
        public IMusician

{
public:
    HumanMusician ();
    ~HumanMusician() override;
    void unloadComponent () override final;
    void play () override;
    void eat () override;
    void sleep () override;
    void kiss () override;
    void work () override;

    void learn () override;
    void playMusic (const MusicScore & score) override;
    void listen () override;
    void practice () override;
    void party () override;

private:
    std::string m_name;
};

}
#endif
