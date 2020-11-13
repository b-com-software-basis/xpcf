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

#ifndef IGUITARIST_H
#define IGUITARIST_H
#include "IMusician.h"
#include "IGuitar.h"
/**
 *  @ingroup interfaces
 */

/**
   * @class IGuitarist
   * This interface declares a set of methods describing a guitarist.
   * It inherits from org::bcom::xpcf::IComponentIntrospect to provide introspection capabilities.
   *
   * @note The InterfaceTraits<IGuitarist> template struct defines the IGuitarist UUID and description.
   * It allows IGuitarist users to use directly the IGuitarist type instead of its UUID to bind the component to this interface.
   */
class IGuitarist : virtual public IMusician
{
public:
    virtual ~IGuitarist() = default;
    virtual void playSolo (const MusicScore & score) = 0;
    virtual void playRhythm () = 0;
    virtual SRef<IGuitar> getGuitar() = 0;
    virtual SRef<IGuitar> getGuitar(IGuitar::GuitarType type) = 0;
    virtual const org::bcom::xpcf::IEnumerable<SRef<IElectricGuitar>> & getGuitarCollection() = 0;
};

template <> struct org::bcom::xpcf::InterfaceTraits<IGuitarist>
{
    static constexpr const char * UUID = "{41348765-1017-47a7-ab9F-6b59d39e4b95}";
    static constexpr const char * NAME = "IGuitarist";
    static constexpr const char * DESCRIPTION = "provides guitar player specific methods";
};

#endif // IGUITARIST_H
