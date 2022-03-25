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

#include "xpcf/api/InjectableMetadata.h"
#include "private/xpcf/PathBuilder.h"
#include <xpcf/collection/Collection.h>
#include <string.h>

#include <boost/uuid/uuid_generators.hpp>
// implement Component
//

namespace org { namespace bcom { namespace xpcf {

using namespace uuids;
class InjectableMetadata::InjectableMetadataImpl {
public:
    InjectableMetadataImpl(const uuids::uuid & serviceUUID, bool optional);
    InjectableMetadataImpl(const uuids::uuid & serviceUUID, const char * name, bool optional);
    InjectableMetadataImpl(const InjectableMetadataImpl & copy ) = default;
    bool m_bOptional = false;
    uuids::uuid m_serviceUUID;
    std::string m_name;
    const std::function<void(SRef<IComponentIntrospect>)> m_injector;
    bool m_bNamed = false;

};

InjectableMetadata::InjectableMetadataImpl::InjectableMetadataImpl(const uuids::uuid & serviceUUID,
                                                                   bool optional ) :
    m_bOptional(optional),m_serviceUUID(serviceUUID)
{
}

InjectableMetadata::InjectableMetadataImpl::InjectableMetadataImpl(const uuids::uuid & serviceUUID,
                                                                   const char * name,
                                                                   bool optional ) :
    InjectableMetadata::InjectableMetadataImpl(serviceUUID,optional)
{
    m_name = name;
    m_bNamed = true;
}

InjectableMetadata::InjectableMetadata(const uuids::uuid & serviceUUID, bool optional ) :
    m_pimpl(new InjectableMetadataImpl( serviceUUID, optional)) {

}

InjectableMetadata::InjectableMetadata(const uuids::uuid & serviceUUID, const char * name, bool optional ) :
    m_pimpl(new InjectableMetadataImpl( serviceUUID, name, optional)) {

}

InjectableMetadata::~InjectableMetadata() {}

bool InjectableMetadata::isNamed()
{
    return m_pimpl->m_bNamed;
}

bool InjectableMetadata::optional() const { return m_pimpl->m_bOptional; }

const char * InjectableMetadata::getName() const { return m_pimpl->m_name.c_str(); }

const uuids::uuid & InjectableMetadata::getUuid() const { return m_pimpl->m_serviceUUID; }

class Injector::InjectorImpl {
public:
    InjectorImpl(const std::function<void(SRef<IComponentIntrospect>)> & injector);
    InjectorImpl(const InjectorImpl & copy ) = default;
    InjectorImpl(const std::function<void(SRef<IEnumerable<SRef<IComponentIntrospect>>>)> & injector);
    const std::function<void(SRef<IComponentIntrospect>)> m_injector;
    const std::function<void(SRef<IEnumerable<SRef<IComponentIntrospect>>>)> m_multiInjector;
    bool m_isMulti;

};

Injector::InjectorImpl::InjectorImpl( const std::function<void(SRef<IComponentIntrospect>)> & injector): m_injector(injector),m_isMulti(false)
{
}

Injector::InjectorImpl::InjectorImpl( const std::function<void(SRef<IEnumerable<SRef<IComponentIntrospect>>>)> & injector): m_multiInjector(injector),m_isMulti(true)
{
}

Injector::Injector( const std::function<void(SRef<IComponentIntrospect>)> & injector, uuids::uuid serviceUUID,  bool optional ):
    InjectableMetadata(serviceUUID,optional), m_pimpl(new InjectorImpl(injector))
{
}

Injector::Injector( const std::function<void(SRef<IComponentIntrospect>)> & injector, uuids::uuid serviceUUID, const char * name, bool optional ):
    InjectableMetadata(serviceUUID,name,optional), m_pimpl(new InjectorImpl(injector))
{
}

Injector::Injector( const std::function<void(SRef<IEnumerable<SRef<IComponentIntrospect>>>)> & injector, uuids::uuid serviceUUID,  bool optional ):
    InjectableMetadata(serviceUUID,optional), m_pimpl(new InjectorImpl(injector))
{
}

Injector::Injector( const std::function<void(SRef<IEnumerable<SRef<IComponentIntrospect>>>)> & injector, uuids::uuid serviceUUID, const char * name, bool optional ):
    InjectableMetadata(serviceUUID,name,optional), m_pimpl(new InjectorImpl(injector))
{
}

Injector::~Injector() {}

void Injector::inject(SRef<IComponentIntrospect> instance)
{
    m_pimpl->m_injector(instance);
}

void Injector::inject(SRef<IEnumerable<SRef<IComponentIntrospect>>> instance)
{
    m_pimpl->m_multiInjector(instance);
}

bool Injector::isMulti()
{
    return m_pimpl->m_isMulti;
}

}}} //namespace org::bcom::xpcf
