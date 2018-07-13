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

#ifndef I2_H
#define I2_H
#include "I1.h"

/**
 *  @ingroup interfaces
 */
class I2 : public virtual I1
{
public:
    virtual ~I2() = default;
    virtual void I2_opname () = 0;
    virtual void I2_opname2 () = 0;
};

template <> struct org::bcom::xpcf::InterfaceTraits<I2>
{
    static constexpr const char * UUID = "41348765-1017-47a7-ab9F-6b59d39e4b95";
    static constexpr const char * DESCRIPTION = "I2 interface";
 };

#endif // I2_H
