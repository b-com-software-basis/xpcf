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
 * @date 2017-12-11
 */

#include "Collection.h"
#include <string>
#include <iostream>
#include <vector>

namespace xpcf=org::bcom::xpcf;
template class xpcf::IEnumerator<xpcf::uuids::uuid>;
template class xpcf::IEnumerable<xpcf::uuids::uuid>;
template class xpcf::Enumerator<xpcf::uuids::uuid, std::vector>;
template class xpcf::ICollection<xpcf::uuids::uuid>;
template class xpcf::Collection<xpcf::uuids::uuid, std::vector>;
