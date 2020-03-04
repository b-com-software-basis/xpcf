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
 * @date 2018-07-13
 */

#ifndef ORG_BCOM_XPCF_ICLONEABLE_H
#define ORG_BCOM_XPCF_ICLONEABLE_H
#include "IComponentIntrospect.h"

namespace org { namespace bcom { namespace xpcf {

class ICloneable : virtual public IComponentIntrospect {
public:
  virtual ~ICloneable() = default;
  //deep clone
  virtual SRef<IComponentIntrospect> clone () = 0;
};

template <> struct InterfaceTraits<ICloneable>
{
    static constexpr const char * UUID = "79CD3C44-5418-4FBF-945A-2D841A05A369";
    static constexpr const char * DESCRIPTION = "XPCF::ICloneable interface";
};

}}}

#endif // ICLONEABLE_H
