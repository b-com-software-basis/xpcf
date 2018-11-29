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

#include "xpcf/component/ConfigurableBase.h"
#include "PropertyManager.h"
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

class ConfigurableBase::InternalImpl {

public:
    InternalImpl() {}
    ~InternalImpl() = default;

    inline boost::log::sources::severity_logger< boost::log::trivial::severity_level > & getLogger() { return m_logger; }
    inline SRef<IPropertyMap> getPropertyRootNode() const { return m_parameters; }

    // IEnumerable
    /*   inline UniqueRef<IEnumerator<uuids::uuid>> getEnumerator() {
        return unixpcf::make_unique<Enumerator<uuids::uuid, vector>>(m_interfacesUUID);
    }

    inline uint32_t size() { return m_interfacesUUID.size(); }*/

private:
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;
    InternalImpl(InternalImpl const &);
    InternalImpl & operator=(InternalImpl const &);

    SRef<IPropertyMap> m_parameters = getPropertyMapInstance();
};

ConfigurableBase::ConfigurableBase(const uuids::uuid & uuid,
                             const SRef<IPropertyMap> configuration)
    :ComponentBase(uuid), m_internalImpl(new InternalImpl())
{
    m_internalImpl->getLogger().add_attribute("ClassName", boost::log::attributes::constant<std::string>("ConfigurableBase"));
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<" ConfigurableBase::ConfigurableBase construction";
    addInterface<IConfigurable>(this);
}

ConfigurableBase::ConfigurableBase(std::map<std::string,std::string> componentTrait,
                             const SRef<IPropertyMap> configuration)
    :ComponentBase(componentTrait) ,m_internalImpl(new InternalImpl())
{
    m_internalImpl->getLogger().add_attribute("ClassName", boost::log::attributes::constant<std::string>("ConfigurableBase"));
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<" ConfigurableBase::ConfigurableBase construction";
    addInterface<IConfigurable>(this);
}

ConfigurableBase::~ConfigurableBase()
{
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<" ConfigurableBase::~ConfigurableBase destruction";
}

XPCFErrorCode ConfigurableBase::serialize(const char * filepath, uint32_t mode)
{
    SRef<xpcf::IPropertyManager> xpcfPropertyManager = xpcf::getPropertyManagerInstance();
    return xpcfPropertyManager->serialize(getUUID(), this->bindTo<IConfigurable>(), filepath, mode);
}

XPCFErrorCode ConfigurableBase::configure(const char * filepath)
{
    SRef<xpcf::IPropertyManager> xpcfPropertyManager = xpcf::getPropertyManagerInstance();
    XPCFErrorCode result = xpcfPropertyManager->configure(getUUID(), this->bindTo<IConfigurable>(), filepath);
    if (result != XPCFErrorCode::_SUCCESS) {
        return result;
    }
    return onConfigured();
}

XPCFErrorCode ConfigurableBase::configure(const char * filepath, const char * xpath)
{
    SRef<xpcf::IPropertyManager> xpcfPropertyManager = xpcf::getPropertyManagerInstance();
    XPCFErrorCode result = xpcfPropertyManager->configure(xpath, getUUID(), this->bindTo<IConfigurable>(), filepath);
    if (result != XPCFErrorCode::_SUCCESS) {
        return result;
    }
    return onConfigured();
}

SRef<IPropertyMap> ConfigurableBase::getPropertyRootNode() const
{
    return m_internalImpl->getPropertyRootNode();
}

SRef<IProperty> ConfigurableBase::getProperty(const char * name) const
{
    return m_internalImpl->getPropertyRootNode()->at(name);
}

bool ConfigurableBase::hasProperties() const
{
    return (m_internalImpl->getPropertyRootNode()->getProperties().size() != 0);
}

SRef<IEnumerator<SRef<IProperty>>> ConfigurableBase::getPropertiesEnumerator() const
{
    return m_internalImpl->getPropertyRootNode()->getProperties().getEnumerator();
}

IEnumerable<SRef<IProperty>> & ConfigurableBase::getProperties() const
{
    return m_internalImpl->getPropertyRootNode()->getProperties();
}

}}} //namespace org::bcom::xpcf
