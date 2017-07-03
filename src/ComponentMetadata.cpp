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

#include "ComponentMetadata.h"

#include <string.h>

#include <boost/uuid/uuid_generators.hpp>
// implement Component
//



namespace org {
namespace bcom {
namespace xpcf {
using namespace utils::uuids;

ComponentMetadata::ComponentMetadata(const char* name, const uuid& componentID, const uuid& containerUUID):InterfaceMetadata(name,componentID),m_containerUUID(containerUUID)
{
}

ComponentMetadata::ComponentMetadata(const char *name, const char *componentID, const char *containerUUID):InterfaceMetadata(name,componentID)
{
    setContainerUUID(containerUUID);
}

ComponentMetadata::~ComponentMetadata()
{
}

void ComponentMetadata::setContainerUUID(const char* containerUUID)
{
    if (containerUUID != nullptr) {
        string_generator gen;
        m_containerUUID = gen(containerUUID);
    }
}


void ComponentMetadata::addInterface(const uuid& interfaceUUID)
{
    m_interfaceUUIDs.push_back(interfaceUUID);
}

uuid ComponentMetadata::getInterface(int i) const
{
    return m_interfaceUUIDs[i];
}

int ComponentMetadata::getNbInterfaces() const
{
    return m_interfaceUUIDs.size();
}

uuid ComponentMetadata::getContainerUUID() const
{
    return m_containerUUID;
}

}}} //namespace org::bcom::xpcf
