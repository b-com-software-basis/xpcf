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

#include "xpcf/component/ComponentBase.h"
#include "PropertyManager.h"
#include "xpcf/core/Exception.h"
#include <iostream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include "Collection.h"
#include <string>

using namespace std;

namespace logging = boost::log;

namespace org { namespace bcom { namespace xpcf {

template class IEnumerable<uuids::uuid>;

class ComponentBase::InternalImpl {

public:
    InternalImpl(const uuids::uuid& componentUUID):m_componentUUID(componentUUID) {}
    ~InternalImpl() = default;
    void addInterface(uuids::uuid& interfaceUUID, utils::any aThis, const char * name);
    const char* getDescription(const uuids::uuid& interfaceUUID) const;
    utils::any queryInterface(const uuids::uuid& interfaceUUID) const;
    bool implements(const uuids::uuid& interfaceUUID) const;

    uint32_t getNbInterfaces() const;
    const IEnumerable<uuids::uuid> & getInterfaces() const { return m_interfacesUUID; }

    inline boost::log::sources::severity_logger< boost::log::trivial::severity_level > & getLogger() { return m_logger; }
    // IEnumerable
    /*   inline UniqueRef<IEnumerator<uuids::uuid>> getEnumerator() {
        return unixpcf::make_unique<Enumerator<uuids::uuid, vector>>(m_interfacesUUID);
    }

    inline uint32_t size() { return m_interfacesUUID.size(); }*/

private:
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;
    InternalImpl(InternalImpl const &);
    InternalImpl & operator=(InternalImpl const &);

    struct XPCF_ObjectInterface {
        uuids::uuid m_uuid;
        utils::any m_component;
        std::string m_description;
    };

    Collection<uuids::uuid,vector> m_interfacesUUID;
    std::map<uuids::uuid, UniqueRef<XPCF_ObjectInterface>> m_interfaces;
    const uuids::uuid & m_componentUUID;
};

ComponentBase::ComponentBase(const uuids::uuid & uuid)
    :m_UUID(uuid), m_internalImpl(new InternalImpl(uuid)), m_usageRefCount(0)
{
    m_internalImpl->getLogger().add_attribute("ClassName", boost::log::attributes::constant<std::string>("ComponentBase"));
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<uuids::to_string(m_UUID)<<" ComponentBase::ComponentBase construction";
    addInterface<IComponentIntrospect>(this);
}

ComponentBase::ComponentBase(std::map<std::string,std::string> componentTrait)
    : m_UUID(toUUID(componentTrait.at("UUID"))), m_componentTrait(componentTrait),
      m_internalImpl(new InternalImpl(toUUID(componentTrait.at("UUID")))), m_usageRefCount(0)
{
    m_internalImpl->getLogger().add_attribute("ClassName", boost::log::attributes::constant<std::string>("ComponentBase"));
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<uuids::to_string(m_UUID)<<" ComponentBase::ComponentBase construction";
    addInterface<IComponentIntrospect>(this);
}

ComponentBase::~ComponentBase()
{
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<uuids::to_string(m_UUID)<<" ComponentBase::~ComponentBase destruction";
}

void ComponentBase::addComponentRef()
{
    // NOTE : add/releaseComponentRef not synchronized !
    m_usageRefCount++;
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<uuids::to_string(m_UUID)<<" ComponentBase::addComponentRef refcount="<<m_usageRefCount;
}

void ComponentBase::releaseComponentRef()
{
    m_usageRefCount--;
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<uuids::to_string(m_UUID)<<" ComponentBase::releaseComponentRef refcount="<<m_usageRefCount;
    if (m_usageRefCount == 0) {
        BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<uuids::to_string(m_UUID)<<" ComponentBase::releaseComponentRef calling unloadComponent";
        this->unloadComponent();
    }
}

uint32_t ComponentBase::getNbInterfaces() const
{
    return m_internalImpl->getInterfaces().size();
}

const IEnumerable<uuids::uuid> & ComponentBase::getInterfaces() const
{
    return m_internalImpl->getInterfaces();
}

SRef<IComponentIntrospect> ComponentBase::introspect()
{
    return IComponentIntrospect::acquireComponentRef<remove_pointer<decltype(this)>::type, IComponentIntrospect>(this);
}

const char * ComponentBase::getDescription(const uuids::uuid& interfaceUUID) const
{
    return m_internalImpl->getDescription(interfaceUUID);
}

void ComponentBase::addInterface(uuids::uuid& interfaceUUID, utils::any componentThis, const char * name)
{
    m_internalImpl->addInterface(interfaceUUID, componentThis, name);
}

utils::any ComponentBase::queryInterface(const uuids::uuid& interfaceUUID) const
{
    return m_internalImpl->queryInterface(interfaceUUID);
}

bool ComponentBase::implements(const uuids::uuid& interfaceUUID) const
{
     return m_internalImpl->implements(interfaceUUID);
}

uint32_t ComponentBase::InternalImpl::getNbInterfaces() const
{
    return m_interfaces.size();
}

void ComponentBase::InternalImpl::addInterface(uuids::uuid& interfaceUUID, utils::any componentThis, const char * name)
{
    if (m_interfaces.find(interfaceUUID) == m_interfaces.end()) {
        m_interfaces[interfaceUUID] = unixpcf::make_unique<struct XPCF_ObjectInterface>();
        m_interfaces[interfaceUUID]->m_component = componentThis;
        m_interfaces[interfaceUUID]->m_uuid = interfaceUUID;
        m_interfacesUUID.add(interfaceUUID);
        if (name != nullptr) {
            m_interfaces[interfaceUUID]->m_description = name;
        }
    }
}



utils::any ComponentBase::InternalImpl::queryInterface(const uuids::uuid& interfaceUUID) const
{
    //TODO : handle error case : or document sptr must be tested for validity upon call
    utils::any component;
    if (m_interfaces.find(interfaceUUID) != m_interfaces.end()) {
        component = m_interfaces.at(interfaceUUID)->m_component;
    }
    else {
        throw InterfaceNotImplementedException(m_componentUUID,interfaceUUID);
    }
    return component;
}

bool ComponentBase::InternalImpl::implements(const uuids::uuid& interfaceUUID) const
{
     return (m_interfaces.find(interfaceUUID) != m_interfaces.end());
}


const char * ComponentBase::InternalImpl::getDescription(const uuids::uuid& interfaceUUID) const
{
    if (m_interfaces.find(interfaceUUID) != m_interfaces.end()) {
        return m_interfaces.at(interfaceUUID)->m_description.c_str();
    }
    return nullptr;
}

}}} //namespace org::bcom::xpcf
