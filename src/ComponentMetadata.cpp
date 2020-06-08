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

#include "xpcf/api/ComponentMetadata.h"
#include "PathBuilder.h"
#include <xpcf/collection/Collection.h>
#include <string.h>

#include <boost/uuid/uuid_generators.hpp>
// implement Component
//

namespace org { namespace bcom { namespace xpcf {

using namespace uuids;


class ComponentMetadata::ComponentMetadataImpl {
public:
    ComponentMetadataImpl() = default;
    ComponentMetadataImpl(const ComponentMetadataImpl & copy ) = default;
    Collection<uuids::uuid,std::vector> m_interfaceUUIDs;
};


ComponentMetadata::ComponentMetadata(const char* name, const uuid& componentID, const char* description):
    InterfaceMetadata(name, componentID, description),m_pimpl(new ComponentMetadataImpl())
{
}

ComponentMetadata::ComponentMetadata(const char *name, const char *componentID, const char* description):
    InterfaceMetadata(name, componentID, description),m_pimpl(new ComponentMetadataImpl())
{
}

ComponentMetadata::ComponentMetadata(const ComponentMetadata & copy):InterfaceMetadata (copy),m_pimpl(new ComponentMetadataImpl())
{
    for (auto i : copy.getInterfaces()) {
        m_pimpl->m_interfaceUUIDs.add(i);
    }
}

ComponentMetadata::ComponentMetadata(ComponentMetadata && other):InterfaceMetadata (other)
{
    this->m_pimpl = std::move(other.m_pimpl);
}

ComponentMetadata::~ComponentMetadata()
{
}

ComponentMetadata & ComponentMetadata::operator=(const ComponentMetadata & copy)
{
    InterfaceMetadata::operator=(copy);
    m_pimpl->m_interfaceUUIDs.clear();
    for (auto i : copy.getInterfaces()) {
        m_pimpl->m_interfaceUUIDs.add(i);
    }
    return *this;
}

ComponentMetadata& ComponentMetadata::operator=(ComponentMetadata && other)
{
    this->InterfaceMetadata::operator=(other);
    this->m_pimpl = std::move(other.m_pimpl);
    return *this;
}

bool ComponentMetadata::operator==(const ComponentMetadata & c)
{
    if ((strcmp(c.description(),this->description()) ==0) &&
            c.getUUID() == this->getUUID() &&
            (strcmp(c.name(),this->name()) ==0)) {
        return true;
    }
    return false;
}

void ComponentMetadata::addInterface(const uuid& interfaceUUID)
{
    m_pimpl->m_interfaceUUIDs.add(interfaceUUID);
}

const IEnumerable<uuids::uuid> & ComponentMetadata::getInterfaces() const
{
    return m_pimpl->m_interfaceUUIDs;
}

}}} //namespace org::bcom::xpcf
