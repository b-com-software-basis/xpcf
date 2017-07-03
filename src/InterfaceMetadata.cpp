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
 * @date 2017-04-28
 */

#include "InterfaceMetadata.h"
#include <string.h>
#include <boost/uuid/uuid_generators.hpp>



namespace org {
namespace bcom {
namespace xpcf {

using namespace utils::uuids;

InterfaceMetadata::InterfaceMetadata(const char* name, const uuid& interfaceUUID):m_description(name),m_uuid(interfaceUUID)
{
}

InterfaceMetadata::InterfaceMetadata(const char* name, const char * interfaceUUID):m_description(name)
{
    setUUID(interfaceUUID);
}

void InterfaceMetadata::setUUID(const char *elementUUID)
{
    if (elementUUID != nullptr) {
        string_generator gen;
        m_uuid = gen(elementUUID);
    }
}

}}} //namespace org::bcom::xpcf
