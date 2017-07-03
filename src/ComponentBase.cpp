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

#define BOOST_ALL_DYN_LINK 1
#include "ComponentBase.h"
#include <string.h>
#include <iostream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/move/make_unique.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include "IComponentManager.h"

using namespace std;

namespace logging = boost::log;

namespace org {
namespace bcom {
namespace xpcf {

class ComponentBase::InternalImpl {

public:
    InternalImpl() = default;
    ~InternalImpl() = default;
    void addInterface(uuids::uuid& interfaceUUID, utils::any aThis, const char * name);
    const char* getDescription(const uuids::uuid& interfaceUUID) const;
    utils::any queryInterface(const uuids::uuid& interfaceUUID) const;
    int getNbInterfaces() const;
    void getInterfaces(uuids::uuid* anInterfacesArray) const;
    inline boost::log::sources::severity_logger< boost::log::trivial::severity_level > & getLogger() { return m_logger;}

private:
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;
    InternalImpl(InternalImpl const &);
    InternalImpl & operator=(InternalImpl const &);

    struct XPCF_ObjectInterface {
        uuids::uuid	m_uuid;
        utils::any	m_component;
        std::string		m_description;
    };

    std::map<uuids::uuid, UniqueRef<XPCF_ObjectInterface>> m_interfaces;
};

ComponentBase::ComponentBase():m_internalImpl(new InternalImpl()),m_count(0)
{
    m_internalImpl->getLogger().add_attribute("ClassName", boost::log::attributes::constant<std::string>("ComponentBase"));
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<"ComponentBase::ComponentBase construction";
    addInterface<IComponentIntrospect>(this,IComponentIntrospect::UUID, "IComponentIntrospect");
}

ComponentBase::~ComponentBase()
{
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<"ComponentBase::~ComponentBase destruction";
}

void ComponentBase::addComponentRef() {
    m_count++;
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<"ComponentBase::addComponentRef refcount="<<m_count;
}

void ComponentBase::releaseComponentRef() {
    m_count--;
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<"ComponentBase::releaseComponentRef refcount="<<m_count;
    if (m_count == 0) {
        BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<"ComponentBase::releaseComponentRef calling unloadComponent";
        this->unloadComponent();
    }
}

int ComponentBase::getNbInterfaces() const
{
    return m_internalImpl->getNbInterfaces();
}

SRef<IComponentIntrospect>     ComponentBase::introspect()
{
    return IComponentIntrospect::acquireComponentRef<remove_pointer<decltype(this)>::type,IComponentIntrospect>(this);
}

const char * ComponentBase::getDescription(const uuids::uuid& interfaceUUID) const
{
    return m_internalImpl->getDescription(interfaceUUID);
}

void ComponentBase::getInterfaces(uuids::uuid* interfacesArray) const
{
    m_internalImpl->getInterfaces(interfacesArray);
}


void ComponentBase::addInterface(uuids::uuid& interfaceUUID, utils::any componentThis, const char * name)
{
    m_internalImpl->addInterface(interfaceUUID, componentThis, name);
}

void ComponentBase::setUUID(const uuids::uuid& componentUUID)
{
    m_UUID = componentUUID;
}

void ComponentBase::setUUID(const char * componentUUID)
{
    uuids::uuid componentID = toUUID(componentUUID );
    setUUID(componentID);
}

utils::any ComponentBase::queryInterface(const uuids::uuid& interfaceUUID) const
{
    return m_internalImpl->queryInterface(interfaceUUID);
}

int ComponentBase::InternalImpl::getNbInterfaces() const
{
    return m_interfaces.size();
}

void ComponentBase::InternalImpl::getInterfaces(uuids::uuid* interfacesArray) const
{
    if (interfacesArray) {
        unsigned long i = 0;
        for (auto & compInterface:m_interfaces) {
            interfacesArray[i++] = compInterface.first;
        }
    }
}

void ComponentBase::InternalImpl::addInterface(uuids::uuid& interfaceUUID, utils::any componentThis, const char * name)
{
    if (m_interfaces.find(interfaceUUID) == m_interfaces.end()) {
        m_interfaces[interfaceUUID] = unixpcf::make_unique<struct XPCF_ObjectInterface>();
        m_interfaces[interfaceUUID]->m_component = componentThis;
        m_interfaces[interfaceUUID]->m_uuid = interfaceUUID;
        if(name != nullptr) {
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
    return component;
}

const char * ComponentBase::InternalImpl::getDescription(const uuids::uuid& interfaceUUID) const
{
    if (m_interfaces.find(interfaceUUID) != m_interfaces.end()) {
        return m_interfaces.at(interfaceUUID)->m_description.c_str();
    }
    return nullptr;
}

}}} //namespace org::bcom::xpcf
