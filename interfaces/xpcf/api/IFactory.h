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

namespace org { namespace bcom { namespace xpcf {


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

typedef enum {
    Empty,
    Cloned,
    Shared
} ContextMode;


class IFactory : virtual public IComponentIntrospect {
public:
    virtual ~IFactory() override = default;
    virtual void clear() = 0;
    virtual SRef<IFactory> createNewFactoryContext(ContextMode ctxMode = ContextMode::Empty) = 0;


    /**
     *
     * @param [in] componentUUID
     * @return
     * @throws InjectableNotFoundException when the component declares injectable(s) and there was missing bind(s) to resolve them
     */
    virtual SRef<IComponentIntrospect> createComponent(const uuids::uuid & componentUUID) = 0;

    /**
     *
     * @note with @fn createComponent()
     * @param [in]
     * @param [in]
     * @return
     * @throws InjectableNotFoundException when the component declares injectable(s) and there was missing bind(s) to resolve them
     */
    virtual SRef<IComponentIntrospect> createComponent(const char * instanceName, const uuids::uuid & componentUUID) = 0;

    /**
     * Search the registry file and load it with all the components
     * The method tries to read the "XPCF_REGISTRY_PATH" environment variable
     * It loads the file provided in the variable when it exists.
     * Otherwise, the method loads all xpcf compatible xml files recursively found in xpcf home path (%USERPROFILE%/.xpcf on windows, $HOME/.xpcf on other platforms).
     * @return
     */
    virtual XPCFErrorCode load() = 0;

    /**
     * Search the registry file and load it with all the components
     * @return
     */
    virtual XPCFErrorCode load(const char* libraryFilePath) = 0;

    /**
     * Find registry files from a root folder and load each registry file with its components
     * @param [in] folderPathStr : the root path to search xml registry's file for
     * @param [in] bRecurse : indicates to search recursively in subfolder [true] or only in @p folderPathStr [false]
     * @return
     */
    virtual XPCFErrorCode load(const char* folderPathStr, bool bRecurse) = 0;

    /**
     * Declare a binding from the service identified with @p interfaceUUID to the concrete component identified with @p instanceUUID.
     * @param [in] interfaceUUID : the interface identifier
     * @param [in] instanceUUID : the component identifier
     * @param [in] scope : the creation scope used to determine the lifetime of the object retrieved with resolve
     * @note bindings can come from in-code calls to bind, from autobinds or bindings declared in an xml configuration file or from autobinds while introspecting a module
     */
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

    virtual SRef<IComponentIntrospect> resolve(const SPtr<InjectableMetadata> & injectableInfo) = 0;

    /**
     *
     * @note with @fn resolve()
     * @param [in]
     * @return
     * @throws  InjectableNotFoundExceptionwhen no bind was found to resolve an instance for I
     * or when the component resolved for I declares injectable(s) and there was missing bind(s) to resolve them
     */
    virtual SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID) = 0;

    /**
     *
     * @note with @fn resolve()
     * @param [in]
     * @param [in]
     * @return
     * @throws InjectableNotFoundException when no bind was found to resolve an instance for {I, name}
     * or when the component resolved for {I, name} declares injectable(s) and there was missing bind(s) to resolve them
     */
    virtual SRef<IComponentIntrospect> resolve(const uuids::uuid & interfaceUUID, const std::string & name) = 0;
    virtual const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const SPtr<InjectableMetadata> & injectableInfo) = 0;
    virtual const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll(const uuids::uuid & interfaceUUID) = 0;
    virtual uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID) = 0;
    virtual uuids::uuid getComponentUUID(const uuids::uuid & interfaceUUID, const std::string & name) = 0;

    /**
     *
     * @note with @fn createComponent()
     * @param [in]
     * @return
     * @throws InjectableNotFoundException when the component declares injectable(s) and there was missing bind(s) to resolve them
     */
    template < typename I> SRef<I> createComponent(const uuids::uuid & componentUUID);

    /**
     *
     * @note with @fn createComponent()
     * @param [in]
     * @param [in]
     * @return
     * @throws InjectableNotFoundException when the component declares injectable(s) and there was missing bind(s) to resolve them
     */
    template < typename I> SRef<I> createComponent(const char * instanceName, const uuids::uuid & componentUUID);
    /**
     *
     * @note with @fn resolve()
     * @return
     * @throws InjectableNotFoundException when the component declares injectable(s) and there was missing bind(s) to resolve them
     */
    template < typename C> SRef<IComponentIntrospect> create();

    /**
     *
     * @note with @fn resolve()
     * @param [in]
     * @return
     * @throws InjectableNotFoundException when the component declares injectable(s) and there was missing bind(s) to resolve them
     */
    template < typename C> SRef<IComponentIntrospect> create(const char * instanceName);

    template <typename I> SRef<I> resolve();
    template <typename I> SRef<I> resolve(const std::string & name);
    template < typename I> const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll();
    template < typename I, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Default|BindingRange::All > void bind(const uuids::uuid& componentUUID);

    template < typename T, typename I, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit> void bind(const uuids::uuid& componentUUID);

    template < typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Default|BindingRange::All > void bind();

    template < typename T, typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit> void bind();

    template < typename I, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Named > void bind(const char * name, const uuids::uuid& componentUUID);

    template < typename T, typename I, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit > void bind(const char * name, const uuids::uuid& componentUUID);

    template < typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Named > void bind(const char * name);

    template < typename T, typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit > void bind(const char * name);
    template < typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Default|BindingRange::All  > void bindLocal();
    template < typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Named  > void bindLocal(const char * name);
    template < typename T, typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit > void bindLocal();
    template < typename T, typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit > void bindLocal(const char * name);


};


template < typename I >
SRef<I> IFactory::createComponent(const uuids::uuid& componentUUID)
{
    SRef<IComponentIntrospect> rICIntrospect = createComponent(componentUUID);
    return rICIntrospect->bindTo<I>();
}

template < typename C >
SRef<IComponentIntrospect> IFactory::create()
{
    return createComponent(toUUID<C>());
}

template < typename I >
SRef<I> IFactory::createComponent(const char * instanceName, const uuids::uuid & componentUUID)
{
    SRef<IComponentIntrospect> rICIntrospect = createComponent(instanceName,componentUUID);
    return rICIntrospect->bindTo<I>();
}

template < typename C >
SRef<IComponentIntrospect> IFactory::create(const char * instanceName)
{
    return createComponent(instanceName,toUUID<C>());
}

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

template < typename I>
const SRef<IEnumerable<SRef<IComponentIntrospect>>> IFactory::resolveAll()
{
    return resolveAll(toUUID<I>());
}

template < typename I, BindingScope scope,
           uint8_t bindingRangeMask> void  IFactory::bind(const uuids::uuid & componentUUID)
{
    bind(toUUID<I>(), componentUUID, scope, bindingRangeMask);
}

template < typename T, typename I, BindingScope scope,
           uint8_t bindingRangeMask> void  IFactory::bind(const uuids::uuid & componentUUID)
{
    bind(toUUID<T>(), toUUID<I>(), componentUUID, scope, bindingRangeMask);
}

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void  IFactory::bind()
{

    bind<I,scope,bindingRangeMask>(toUUID<C>());
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask > void IFactory::bind()
{
    bind<T,I,scope,bindingRangeMask>(toUUID<C>());
}

template < typename I, BindingScope scope,
           uint8_t bindingRangeMask> void  IFactory::bind(const char * name, const uuids::uuid & componentUUID)
{
    bind(name, toUUID<I>(), componentUUID, scope, bindingRangeMask);
}

template < typename T, typename I, BindingScope scope,
           uint8_t bindingRangeMask> void  IFactory::bind(const char * name, const uuids::uuid & componentUUID)
{
    bind(toUUID<T>(), name, toUUID<I>(), componentUUID, scope);
}

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void  IFactory::bind(const char * name)
{
    bind<I,scope,bindingRangeMask>(name, toUUID<C>());
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void  IFactory::bind(const char * name)
{
    bind<T,I,scope,bindingRangeMask>(name, toUUID<C>());
}

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IFactory::bindLocal()
{
    bind(toUUID<I>(), toUUID<C>(), &ComponentFactory::create<C>, scope, bindingRangeMask);
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IFactory::bindLocal()
{
    bind(toUUID<T>(), toUUID<I>(), &ComponentFactory::create<C>, toUUID<C>(), scope, bindingRangeMask);
}

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IFactory::bindLocal(const char * name)
{
    bind(name, toUUID<I>(), toUUID<C>(), &ComponentFactory::create<C>, scope, bindingRangeMask);
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IFactory::bindLocal(const char * name)
{
    bind(toUUID<T>(), name, toUUID<I>(), &ComponentFactory::create<C>, toUUID<C>(), scope, bindingRangeMask);
}

template <> struct InterfaceTraits<IFactory>
{
    static constexpr const char * UUID = "DE0ABC12-7850-4798-848A-07C646881A20";
    static constexpr const char * NAME = "XPCF::IFactory";
    static constexpr const char * DESCRIPTION = "Factory interface.\nProvides binding between interfaces uuid and concrete components uuid for injection patterns";
};


}}} //namespace org::bcom::xpcf

#endif
