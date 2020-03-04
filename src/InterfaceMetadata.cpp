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

#include "xpcf/api/InterfaceMetadata.h"
#include <boost/uuid/uuid_generators.hpp>

namespace org { namespace bcom { namespace xpcf {

using namespace uuids;


class InterfaceMetadata::InterfaceMetadataImpl {
public:
    InterfaceMetadataImpl(const char * name, const uuids::uuid& interfaceUUID, const char * description);
    InterfaceMetadataImpl(const char * name, const char * interfaceUUID, const char * description);
    std::basic_string<char> m_name;
    std::basic_string<char> m_description;
    uuids::uuid m_uuid;
    void setUUID(const char * elementUUID);
};

InterfaceMetadata::InterfaceMetadata(const char* name, const uuid& interfaceUUID, const char * description):
    m_pimpl(new InterfaceMetadataImpl(name,interfaceUUID,description))
{
}

InterfaceMetadata::InterfaceMetadata(const char* name, const char * interfaceUUID, const char * description):
    m_pimpl(new InterfaceMetadataImpl(name,interfaceUUID,description))
{
}

InterfaceMetadata::InterfaceMetadata(const InterfaceMetadata & other)
{
    m_pimpl = unixpcf::make_unique<InterfaceMetadataImpl>(other.name(),other.getUUID(),other.description());
}

InterfaceMetadata::InterfaceMetadata(InterfaceMetadata && other)
{
    this->m_pimpl = std::move(other.m_pimpl);
}

InterfaceMetadata::~InterfaceMetadata()
{
}

const char * InterfaceMetadata::name() const
{
    return m_pimpl->m_name.c_str();
}

const char * InterfaceMetadata::description() const
{
    return m_pimpl->m_description.c_str();
}

uuids::uuid InterfaceMetadata::getUUID() const
{
    return m_pimpl->m_uuid;
}

InterfaceMetadata & InterfaceMetadata::operator=(const InterfaceMetadata & other)
{
    m_pimpl->m_description = other.description();
    m_pimpl->m_name = other.name();
    m_pimpl->m_uuid = other.getUUID();
    return * this;
}

InterfaceMetadata & InterfaceMetadata::operator=(InterfaceMetadata && other)
{
    this->m_pimpl = std::move(other.m_pimpl);
    return * this;
}

bool InterfaceMetadata::operator==(const InterfaceMetadata & c)
{
    if ((strcmp(c.description(),this->description()) ==0) &&
            c.getUUID() == this->getUUID() &&
            (strcmp(c.name(),this->name()) ==0)) {
        return true;
    }
    return false;
}

InterfaceMetadata::InterfaceMetadataImpl::InterfaceMetadataImpl(const char * name, const uuids::uuid& interfaceUUID, const char * description):
    m_name(name),m_uuid(interfaceUUID),m_description(description)
{
}

InterfaceMetadata::InterfaceMetadataImpl::InterfaceMetadataImpl(const char * name, const char * interfaceUUID, const char * description):
    m_name(name),m_description(description)
{
    setUUID(interfaceUUID);
}

void InterfaceMetadata::InterfaceMetadataImpl::setUUID(const char *elementUUID)
{
    if (elementUUID != nullptr) {
        m_uuid = xpcf::toUUID(elementUUID);
    }
}

}}} //namespace org::bcom::xpcf
