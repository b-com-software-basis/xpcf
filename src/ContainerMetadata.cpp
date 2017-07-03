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

#include "ContainerMetadata.h"

#include <string.h>

#include <boost/uuid/uuid_generators.hpp>
// implement Component
//


namespace org {
namespace bcom {
namespace xpcf {

using namespace utils::uuids;

ContainerMetadata::ContainerMetadata(const char* name, const uuid& containerID, const char *containerPath):InterfaceMetadata(name,containerID)
{
    setPath(containerPath);
}

ContainerMetadata::ContainerMetadata(const char *name, const char *containerID, const char *containerPath):InterfaceMetadata(name,containerID)
{
    setPath(containerPath);
}

ContainerMetadata::~ContainerMetadata()
{
}

void ContainerMetadata::setPath(const char* containerPath)
{
    if (containerPath != nullptr) {
        m_containerPath = containerPath;
    }
}

void ContainerMetadata::addComponent(const uuid& componentUUID)
{
    m_componentUUIDs.push_back(componentUUID);
}

uuid ContainerMetadata::getComponent(int i) const
{
    return m_componentUUIDs[i];
}

int ContainerMetadata::getNbComponents() const
{
    return m_componentUUIDs.size();
}

const char *ContainerMetadata::getPath() const
{
    return m_containerPath.c_str();
}

}}} //namespace org::bcom::xpcf
