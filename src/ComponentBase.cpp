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
#include "xpcf/core/helpers.h"
#include "xpcf/api/IModuleManager.h"
#include <iostream>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#include <xpcf/collection/Collection.h>
#include <string>

using namespace std;

namespace logging = boost::log;

namespace org { namespace bcom { namespace xpcf {

template class IEnumerable<uuids::uuid>;

class ComponentBase::InternalImpl {

public:
    InternalImpl(const uuids::uuid& componentUUID):m_componentUUID(componentUUID) {}
    ~InternalImpl() = default;
    void declareInterface(const uuids::uuid & interfaceUUID, utils::any aThis, const char * name, const char * description);
    const InterfaceMetadata & getMetadata(const uuids::uuid& interfaceUUID) const;
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

    Collection<SPtr<Injector>,vector> m_injectablesCollection;
    //std::map<uuids::uuid,std::reference_wrapper<utils::any>> m_injectablesMap;
    std::map<uuids::uuid, utils::any> m_injectablesMap;
    std::map<uuids::uuid, utils::any> m_multiInjectablesMap;
    std::map<std::pair<uuids::uuid, std::string>, utils::any> m_namedInjectablesMap;

private:
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;
    InternalImpl(InternalImpl const &);
    InternalImpl & operator=(InternalImpl const &);

    struct XPCF_ObjectInterface {
        XPCF_ObjectInterface(InterfaceMetadata metadata):m_metadata(metadata){}
        InterfaceMetadata m_metadata;
        utils::any m_component;
    };

    Collection<uuids::uuid,vector> m_interfacesUUID;
    std::map<uuids::uuid, UniqueRef<XPCF_ObjectInterface>> m_interfaces;
    const uuids::uuid & m_componentUUID;
};

ComponentBase::ComponentBase(const uuids::uuid & uuid)
    :m_UUID(uuid), m_pimpl(new InternalImpl(uuid)), m_usageRefCount(0)
{
#ifdef XPCF_WITH_LOGS
    m_pimpl->getLogger().add_attribute("ClassName", boost::log::attributes::constant<std::string>("ComponentBase"));
    BOOST_LOG_SEV(m_pimpl->getLogger(), logging::trivial::info)<<uuids::to_string(m_UUID)<<" ComponentBase::ComponentBase construction";
#endif
    declareInterface<IComponentIntrospect>(this);
    declareInterface<IInjectable>(this);
}

ComponentBase::ComponentBase(std::map<std::string,std::string> componentTrait)
    : ComponentBase(toUUID(componentTrait.at("UUID")))
{
    m_componentTrait = componentTrait;
}

ComponentBase::~ComponentBase()
{
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_pimpl->getLogger(), logging::trivial::info)<<uuids::to_string(m_UUID)<<" ComponentBase::~ComponentBase destruction";
#endif
}

void ComponentBase::addComponentRef()
{
    // NOTE : add/releaseComponentRef not synchronized !
    m_usageRefCount++;
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_pimpl->getLogger(), logging::trivial::info)<<uuids::to_string(m_UUID)<<" ComponentBase::addComponentRef refcount="<<m_usageRefCount;
#endif
}

void ComponentBase::releaseComponentRef()
{
    m_usageRefCount--;
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_pimpl->getLogger(), logging::trivial::info)<<uuids::to_string(m_UUID)<<" ComponentBase::releaseComponentRef refcount="<<m_usageRefCount;
#endif
    if (m_usageRefCount == 0) {
#ifdef XPCF_WITH_LOGS
        BOOST_LOG_SEV(m_pimpl->getLogger(), logging::trivial::info)<<uuids::to_string(m_UUID)<<" ComponentBase::releaseComponentRef calling unloadComponent";
#endif
        this->unloadComponent();
    }
}

uint32_t ComponentBase::getNbInterfaces() const
{
    return m_pimpl->getInterfaces().size();
}

const IEnumerable<uuids::uuid> & ComponentBase::getInterfaces() const
{
    return m_pimpl->getInterfaces();
}

const InterfaceMetadata & ComponentBase::getMetadata(const uuids::uuid& interfaceUUID) const
{
    return m_pimpl->getMetadata(interfaceUUID);
}

SRef<IComponentIntrospect> ComponentBase::introspect()
{
    return IComponentIntrospect::acquireComponentRef<remove_pointer<decltype(this)>::type, IComponentIntrospect>(this);
}

const char * ComponentBase::getDescription(const uuids::uuid& interfaceUUID) const
{
    return m_pimpl->getMetadata(interfaceUUID).description();
}

void ComponentBase::declareInterface(const uuids::uuid & interfaceUUID, utils::any componentThis, const char * name, const char * description)
{
    m_pimpl->declareInterface(interfaceUUID, componentThis, name, description);
}

utils::any ComponentBase::queryInterface(const uuids::uuid& interfaceUUID) const
{
    return m_pimpl->queryInterface(interfaceUUID);
}

void ComponentBase::declareInjectable(const uuids::uuid & interfaceUUID, utils::any injectable, const std::function<void(SRef<IComponentIntrospect>)> & injector, const char * instanceName, bool optional)
{
    pair<uuids::uuid,string> key = make_pair(interfaceUUID,instanceName);
    SPtr<Injector> injMdata = utils::make_shared<Injector>(injector,interfaceUUID,instanceName,optional);
    if (mapContains(m_pimpl->m_namedInjectablesMap, key)) {
         throw InjectableDeclarationException(injMdata);
    }
    m_pimpl->m_namedInjectablesMap[key] = injectable;
    m_pimpl->m_injectablesCollection.add(injMdata);
}

void ComponentBase::declareInjectable(const uuids::uuid &  interfaceUUID, utils::any injectable, const std::function<void(SRef<IComponentIntrospect>)> & injector, bool optional)
{
    SPtr<Injector> injMdata = utils::make_shared<Injector>(injector,interfaceUUID,optional);
    if (mapContains(m_pimpl->m_injectablesMap, interfaceUUID)) {
         throw InjectableDeclarationException(injMdata);
    }
    m_pimpl->m_injectablesMap[interfaceUUID] = injectable;
    m_pimpl->m_injectablesCollection.add(injMdata);
}

void ComponentBase::declareMultiInjectable(const uuids::uuid& interfaceUUID, utils::any injectable, const std::function<void(SRef<IEnumerable<SRef<IComponentIntrospect>>>)> & injector, bool optional)
{
    SPtr<Injector> injMdata = utils::make_shared<Injector>(injector,interfaceUUID,optional);
    if (mapContains(m_pimpl->m_multiInjectablesMap, interfaceUUID)) {
         throw InjectableDeclarationException(injMdata);
    }
    m_pimpl->m_multiInjectablesMap[interfaceUUID] = injectable;
    m_pimpl->m_injectablesCollection.add(injMdata);
}


utils::any ComponentBase::retrieveInjectable(const uuids::uuid & interfaceUUID) const
{
    if (!mapContains(m_pimpl->m_injectablesMap, interfaceUUID)) {
        SPtr<InjectableMetadata> injMdata = utils::make_shared<InjectableMetadata>(interfaceUUID);
         throw InjectionException(injMdata, XPCFErrorCode::_ERROR_INJECTABLE_UNKNOWN);
    }
    return m_pimpl->m_injectablesMap.at(interfaceUUID);
}

utils::any ComponentBase::retrieveInjectable(const uuids::uuid & interfaceUUID, const char * name) const
{
    pair<uuids::uuid,string> key = make_pair(interfaceUUID,name);
    if (!mapContains(m_pimpl->m_namedInjectablesMap, key)) {
        SPtr<InjectableMetadata> injMdata = utils::make_shared<InjectableMetadata>(interfaceUUID,name);
        throw InjectionException(injMdata, XPCFErrorCode::_ERROR_INJECTABLE_UNKNOWN);
    }
    return m_pimpl->m_namedInjectablesMap.at(key);
}



utils::any ComponentBase::retrieveMultiInjectable(const uuids::uuid & interfaceUUID)
{
    if (!mapContains(m_pimpl->m_multiInjectablesMap, interfaceUUID)) {
        SPtr<InjectableMetadata> injMdata = utils::make_shared<InjectableMetadata>(interfaceUUID);
        throw InjectionException(injMdata, XPCFErrorCode::_ERROR_INJECTABLE_UNKNOWN);
    }
    return m_pimpl->m_multiInjectablesMap.at(interfaceUUID);
}

bool ComponentBase::injectExists(const uuids::uuid & interfaceUUID) const
{
    return mapContains(m_pimpl->m_injectablesMap, interfaceUUID);
}

bool ComponentBase::injectExists(const uuids::uuid & interfaceUUID, const char * name) const
{
    pair<uuids::uuid,string> key = make_pair(interfaceUUID,name);
    return mapContains(m_pimpl->m_namedInjectablesMap, key);
}


const IEnumerable<SPtr<Injector>> & ComponentBase::getInjectables() const
{
    return m_pimpl->m_injectablesCollection;
}

bool ComponentBase::implements(const uuids::uuid& interfaceUUID) const
{
    return m_pimpl->implements(interfaceUUID);
}

uint32_t ComponentBase::InternalImpl::getNbInterfaces() const
{
    return m_interfaces.size();
}

void ComponentBase::InternalImpl::declareInterface(const uuids::uuid & interfaceUUID, utils::any componentThis, const char * name, const char * description)
{
    if (! mapContains(m_interfaces,interfaceUUID)) {
        InterfaceMetadata data(name,interfaceUUID, description);
        m_interfaces[interfaceUUID] = unixpcf::make_unique<struct XPCF_ObjectInterface>(data);
        m_interfaces[interfaceUUID]->m_component = componentThis;
        m_interfacesUUID.add(interfaceUUID);
    }
}

utils::any ComponentBase::InternalImpl::queryInterface(const uuids::uuid& interfaceUUID) const
{
    utils::any component;
    if (mapContains(m_interfaces,interfaceUUID)) {
        component = m_interfaces.at(interfaceUUID)->m_component;
    }
    else {
        throw InterfaceNotImplementedException(m_componentUUID,interfaceUUID);
    }
    return component;
}

bool ComponentBase::InternalImpl::implements(const uuids::uuid& interfaceUUID) const
{
    return mapContains(m_interfaces,interfaceUUID);
}

const InterfaceMetadata & ComponentBase::InternalImpl::getMetadata(const uuids::uuid& interfaceUUID) const
{
    if (! mapContains(m_interfaces,interfaceUUID)) {
        throw InterfaceNotImplementedException("Interface not found");
    }
    return m_interfaces.at(interfaceUUID)->m_metadata;
}


}}} //namespace org::bcom::xpcf
