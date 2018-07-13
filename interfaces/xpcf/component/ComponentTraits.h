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
 * @date 2017-12-10
 */

#ifndef ORG_BCOM_XPCF_COMPONENTTRAITS_H
#define ORG_BCOM_XPCF_COMPONENTTRAITS_H

namespace org { namespace bcom { namespace xpcf {

template <typename T>
struct ComponentTraits
{
    static constexpr const char * UUID = nullptr;
    static constexpr const char * DESCRIPTION = nullptr;
};

template <typename T>
struct is_component
{
    static const bool value = (ComponentTraits<T>::UUID != nullptr);
};

}}}

#endif // COMPONENTTRAITS_H
