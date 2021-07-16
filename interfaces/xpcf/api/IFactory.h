/**
 * @copyright Copyright (c) 2019 B-com http://www.b-com.com/
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
 * @date 2019-06-05
 */

#ifndef ORG_BCOM_XPCF_IFACTORY_H
#define ORG_BCOM_XPCF_IFACTORY_H

//#define BOOST_ALL_DYN_LINK 1
#include <xpcf/api/IInjectable.h>
#include <xpcf/component/ComponentFactory.h>

#ifdef XPCF_WITH_LOGS
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#endif

#include <deque>

namespace org { namespace bcom { namespace xpcf {
enum class ContextType {
    Component,
    Named,
    Specific,
    Multi
};

typedef enum {
    Transient,
    Singleton
} BindingScope;


typedef enum {
    Explicit = 1,
    WithParents = 2,
    Named = 4,
    Default = 8,
    All = 16
} BindingRange;

struct FactoryBindInfos {
    uuids::uuid componentUUID;
    BindingScope scope;
    uint8_t bindingRangeMask = 0;
    std::string properties;
};

using FactoryContext = std::pair<ContextType, FactoryBindInfos>;

inline bool operator==(const FactoryBindInfos& lhs, const FactoryBindInfos& rhs)
{
    return lhs.componentUUID == rhs.componentUUID &&
           lhs.scope == rhs.scope &&
           lhs.properties == rhs.properties;
}

class IFactory : virtual public IComponentIntrospect {
public:
    virtual ~IFactory() override = default;
    virtual void clear() = 0;
    virtual SRef<IFactory> createNewFactoryContext(bool cloneFromMainContext = false) = 0;
    virtual void autobind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID) = 0;
    virtual void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                      BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const std::string & name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                      BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                      const uuids::uuid & instanceUUID, BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
                      const uuids::uuid & instanceUUID, BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                           const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                           BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const std::string & name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                           const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                           BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                      const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                      const uuids::uuid & instanceUUID, BindingScope scope,
                      uint8_t bindingRangeMask) = 0;
    virtual void bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
                      const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                      const uuids::uuid & instanceUUID, BindingScope scope,
                      uint8_t bindingRangeMask) = 0;

    virtual SRef<IComponentIntrospect> resolve(const SPtr<InjectableMetadata> & injectableInfo,
                                               std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID,
                                               std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID, const std::string & name,
                                               std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const SPtr<InjectableMetadata> & injectableInfo,
                                               std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const uuids::uuid & interfaceUUID,
                                       std::deque<FactoryContext> contextLevels = {}) = 0;
    virtual uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID) = 0;
    virtual uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID, const std::string & name) = 0;

    virtual void inject(SRef<IInjectable> component, std::deque<FactoryContext> contextLevels = {}) = 0;
    template <typename I> SRef<I> resolve();
    template <typename I> SRef<I> resolve(const std::string & name);
    template < typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Default|BindingRange::All  > void bindLocal();
    template < typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Named  > void bindLocal(const char * name);
    template < typename T, typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit > void bindLocal();
    template < typename T, typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit > void bindLocal(const char * name);


};


template <typename I>
SRef<I> IFactory::resolve()
{
    return resolve(toUUID<I>())->template bindTo<I>();
}

template <typename I>
SRef<I> IFactory::resolve(const std::string & name)
{
    return resolve(toUUID<I>(), name)->template bindTo<I>();
}

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IFactory::bindLocal()
{
    bind(toUUID<I>(), toUUID<C>(), &ComponentFactory::create<C>, scope, bindingRangeMask);
}

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IFactory::bindLocal(const char * name)
{
    bind(name, toUUID<I>(), toUUID<C>(), &ComponentFactory::create<C>, scope, bindingRangeMask);
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void bindLocal()
{
    bind(toUUID<T>(), toUUID<I>(), toUUID<C>(), &ComponentFactory::create<C>, scope, bindingRangeMask);
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void bindLocal(const char * name)
{
    bind(toUUID<T>(), name, toUUID<C>(), &ComponentFactory::create<C>, scope, bindingRangeMask);
}


template <> struct InterfaceTraits<IFactory>
{
    static constexpr const char * UUID = "DE0ABC12-7850-4798-848A-07C646881A20";
    static constexpr const char * NAME = "XPCF::IFactory";
    static constexpr const char * DESCRIPTION = "Factory interface.\nProvides binding between interfaces uuid and concrete components uuid for injection patterns";
};


}}} //namespace org::bcom::xpcf

#endif