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

#ifndef IMUSICIAN_H
#define IMUSICIAN_H 1

#include "xpcf/api/IComponentIntrospect.h"

struct MusicScore {
    MusicScore() = default;
    ~MusicScore() = default;
    void add(std::string key, float duration)
    {
        m_score.push_back(std::make_pair(key,duration));
    }
    uint32_t m_tempo = 120;
    std::vector<std::pair<std::string, float>> m_score;
};

namespace boost { namespace serialization {

template<class Archive>
inline void serialize(Archive & ar,
                      MusicScore & score,
                      const unsigned int version)
{
    ar & score.m_tempo;
    ar & score.m_score;
}


}} // namespace boost::serialization


/**
 *  @ingroup interfaces
 */
/**
   * @class IMusician
   * This interface declares a set of methods describing a musician.
   * It inherits from org::bcom::xpcf::IComponentIntrospect to provide introspection capabilities.
   *
   * @note The InterfaceTraits<IMusician> template struct defines the IMusician UUID and description.
   * It allows IMusician users to use directly the I0 type instead of its UUID to bind the component to this interface.
   */
class IMusician : virtual public org::bcom::xpcf::IComponentIntrospect
{
public:
    virtual ~IMusician() = default;
    virtual void learn () = 0;
    virtual void playMusic (const MusicScore & score) = 0;
    virtual void listen () = 0;
    virtual void practice () = 0;
    virtual void party () = 0;

};

/**
   * @struct  InterfaceTraits<IMusician> instanciation.
   *
   * The code below shows the instanciation of the InterfaceTraits template struct for IMusician interface type.
   * The syntax uses the XPCF_DEFINE_INTERFACE_TRAITS helper macro.
   *
   * @note although the syntax seems easier, it obfuscates the InterfaceTraits<InterfaceType> expansion.
   */

XPCF_DEFINE_INTERFACE_TRAITS(::IMusician,
                             "3bc8f5ea-ee24-473e-8afd-4f5b1b21c018",
                             "IMusician",
                             "provides musician related methods");

#endif
