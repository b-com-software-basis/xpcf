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
#include <xpcf/collection/Collection.h>
#include "PropertyWrapper.h"
#include "PropertyMap.h"
#include <string>

using namespace std;

namespace logging = boost::log;

namespace org { namespace bcom { namespace xpcf {

// Primary template
template<typename T>
struct deduce_to
{
};

// Specialization for double: U -> double
template<>
struct deduce_to<double>
{
    typedef double InnerType;
};

template<>
struct deduce_to<float>
{
    typedef float InnerType;
};

template<>
struct deduce_to<int32_t>
{
    typedef int32_t InnerType;
};

template<>
struct deduce_to<uint32_t>
{
    typedef uint32_t InnerType;
};

template<>
struct deduce_to<int64_t>
{
    typedef int64_t InnerType;
};

template<>
struct deduce_to<uint64_t>
{
    typedef uint64_t InnerType;
};

template<>
struct deduce_to<SRef<IPropertyMap>>
{
    typedef SRef<IPropertyMap> InnerType;
};

template<>
struct deduce_to<std::string >
{
    typedef const char * InnerType;
};

template<>
struct deduce_to<std::wstring>
{
    typedef const wchar_t * InnerType;
};


template class IEnumerable<uuids::uuid>;

class ConfigurableBase::InternalImpl {

public:
    InternalImpl() {}
    ~InternalImpl() = default;

#ifdef XPCF_WITH_LOGS
    inline boost::log::sources::severity_logger< boost::log::trivial::severity_level > & getLogger() { return m_logger; }
#endif

    inline SRef<IPropertyMap> getPropertyRootNode() const { return m_parameters; }
    SRef<PropertyMap> m_parameters = utils::make_shared<PropertyMap>();
    SRef<IPropertyManager> m_propsMgr;

private:
#ifdef XPCF_WITH_LOGS
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;
#endif
    InternalImpl(InternalImpl const &);
    InternalImpl & operator=(InternalImpl const &);


};

ConfigurableBase::ConfigurableBase(const uuids::uuid & uuid,
                                   const SRef<IPropertyMap> configuration)
    :ComponentBase(uuid), m_internalImpl(new InternalImpl())
{
#ifdef XPCF_WITH_LOGS
    m_internalImpl->getLogger().add_attribute("ClassName", boost::log::attributes::constant<std::string>("ConfigurableBase"));
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<" ConfigurableBase::ConfigurableBase construction";
#endif
    declareInterface<IConfigurable>(this);
    declareInjectable<IPropertyManager>(m_internalImpl->m_propsMgr);
}

ConfigurableBase::ConfigurableBase(std::map<std::string,std::string> componentTrait,
                                   const SRef<IPropertyMap> configuration)
    :ComponentBase(componentTrait) ,m_internalImpl(new InternalImpl())
{
#ifdef XPCF_WITH_LOGS
    m_internalImpl->getLogger().add_attribute("ClassName", boost::log::attributes::constant<std::string>("ConfigurableBase"));
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<" ConfigurableBase::ConfigurableBase construction";
#endif
    declareInterface<IConfigurable>(this);
    declareInjectable<IPropertyManager>(m_internalImpl->m_propsMgr);
}

ConfigurableBase::~ConfigurableBase()
{
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<" ConfigurableBase::~ConfigurableBase destruction";
#endif
}

void ConfigurableBase::onInjected()
{
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_internalImpl->getLogger(), logging::trivial::info)<<" ConfigurableBase::onInjected IPropertyManager component address="<<m_internalImpl->m_propsMgr.get();
#endif
}

XPCFErrorCode ConfigurableBase::serialize(const char * filepath, uint32_t mode)
{
    return m_internalImpl->m_propsMgr->serialize(getUUID(), this->bindTo<IConfigurable>(), filepath, mode);
}

XPCFErrorCode ConfigurableBase::configure(const char * filepath)
{
    XPCFErrorCode result = m_internalImpl->m_propsMgr->configure(getUUID(), this->bindTo<IConfigurable>(), filepath);
    if (result != XPCFErrorCode::_SUCCESS) {
        return result;
    }
    return onConfigured();
}

XPCFErrorCode ConfigurableBase::configure(const char * filepath, const char * xpath)
{
    XPCFErrorCode result = m_internalImpl->m_propsMgr->configure(xpath, getUUID(), this->bindTo<IConfigurable>(), filepath);
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

XPCFErrorCode ConfigurableBase::addProperty(SRef<IProperty> property)
{
    return m_internalImpl->m_parameters->addProperty(property);
}

void ConfigurableBase::declarePropertyNode(const char * name, SRef<IPropertyMap> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    wrappedValue =  xpcf::getPropertyMapInstance();
    m_internalImpl->m_parameters->wrapStructure(name,wrappedValue,iotype);
}

template<typename T>
XPCFErrorCode ConfigurableBase::declareProperty(const char * name, T & wrappedValue, IProperty::AccessSpecifier iotype)
{
    if (name == nullptr) {
        return XPCFErrorCode::_ERROR_INVALID_ARGUMENT;
    }

    SRef<xpcf::IProperty> wrappedRef = utils::make_shared<xpcf::PropertyWrapper<typename deduce_to<T>::InnerType>>(name, wrappedValue, iotype);
    return addProperty(wrappedRef);
}

template <template<typename, typename> class Sequence, typename T>
XPCFErrorCode ConfigurableBase::declarePropertySequence(const char * name, Sequence<T, ::std::allocator<T>> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    if (name == nullptr) {
        return XPCFErrorCode::_ERROR_INVALID_ARGUMENT;
    }

    SRef<xpcf::IProperty> wrappedRef = utils::make_shared<xpcf::PropertySequenceWrapper<Sequence, typename deduce_to<T>::InnerType> >(name, wrappedValue, iotype);
    return addProperty(wrappedRef);
}

template<typename T>
XPCFErrorCode ConfigurableBase::declareProperty(SRef<IPropertyMap> node, const char * name, T & wrappedValue, IProperty::AccessSpecifier iotype)
{
    if (name == nullptr) {
        return XPCFErrorCode::_ERROR_INVALID_ARGUMENT;
    }

    SRef<xpcf::IProperty> wrappedRef = utils::make_shared<xpcf::PropertyWrapper<typename deduce_to<T>::InnerType>>(name, wrappedValue, iotype);
    return node->addProperty(wrappedRef);
}

template <template<typename, typename> class Sequence, typename T>
XPCFErrorCode ConfigurableBase::declarePropertySequence(SRef<IPropertyMap> node, const char * name, Sequence<T, ::std::allocator<T>> & wrappedValue, IProperty::AccessSpecifier iotype)
{
    if (name == nullptr) {
        return XPCFErrorCode::_ERROR_INVALID_ARGUMENT;
    }

    SRef<xpcf::IProperty> wrappedRef = utils::make_shared<xpcf::PropertySequenceWrapper<Sequence, typename deduce_to<T>::InnerType> >(name, wrappedValue, iotype);
    return node->addProperty(wrappedRef);
}

template XPCFErrorCode ConfigurableBase::declareProperty<uint32_t>(const char * name, uint32_t & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<int32_t>(const char * name, int32_t & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<uint64_t>(const char * name, uint64_t & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<int64_t>(const char * name, int64_t & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<float>(const char * name, float & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<double>(const char * name, double & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<std::string>(const char * name, std::string & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<std::wstring>(const char * name, std::wstring & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector, uint32_t>(const char * name, std::vector<uint32_t> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,int32_t>(const char * name, std::vector<int32_t> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,uint64_t>(const char * name, std::vector<uint64_t> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,int64_t>(const char * name, std::vector<int64_t> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,float>(const char * name, std::vector<float> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,double>(const char * name, std::vector<double> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,std::string>(const char * name, std::vector<std::string> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,std::wstring>(const char * name, std::vector<std::wstring> & wrappedValue, IProperty::AccessSpecifier iotype);

template XPCFErrorCode ConfigurableBase::declareProperty<uint32_t>(SRef<IPropertyMap> node, const char * name, uint32_t & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<int32_t>(SRef<IPropertyMap> node, const char * name, int32_t & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<uint64_t>(SRef<IPropertyMap> node, const char * name, uint64_t & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<int64_t>(SRef<IPropertyMap> node, const char * name, int64_t & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<float>(SRef<IPropertyMap> node, const char * name, float & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<double>(SRef<IPropertyMap> node, const char * name, double & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<std::string>(SRef<IPropertyMap> node, const char * name, std::string & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declareProperty<std::wstring>(SRef<IPropertyMap> node, const char * name, std::wstring & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector, uint32_t>(SRef<IPropertyMap> node, const char * name, std::vector<uint32_t> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,int32_t>(SRef<IPropertyMap> node, const char * name, std::vector<int32_t> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,uint64_t>(SRef<IPropertyMap> node, const char * name, std::vector<uint64_t> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,int64_t>(SRef<IPropertyMap> node, const char * name, std::vector<int64_t> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,float>(SRef<IPropertyMap> node, const char * name, std::vector<float> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,double>(SRef<IPropertyMap> node, const char * name, std::vector<double> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,std::string>(SRef<IPropertyMap> node, const char * name, std::vector<std::string> & wrappedValue, IProperty::AccessSpecifier iotype);
template XPCFErrorCode ConfigurableBase::declarePropertySequence<std::vector,std::wstring>(SRef<IPropertyMap> node, const char * name, std::vector<std::wstring> & wrappedValue, IProperty::AccessSpecifier iotype);


}}} //namespace org::bcom::xpcf
