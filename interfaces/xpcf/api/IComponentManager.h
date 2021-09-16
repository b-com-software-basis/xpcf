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

#ifndef ORG_BCOM_XPCF_ICOMPONENTMANAGER_H
#define ORG_BCOM_XPCF_ICOMPONENTMANAGER_H

#include <xpcf/api/IInjectable.h>
#include <xpcf/api/IFactory.h>
#include <xpcf/api/InterfaceMetadata.h>
#include <xpcf/api/ComponentMetadata.h>
#include <xpcf/api/ModuleMetadata.h>
#include <xpcf/core/XPCFErrorCode.h>
#include <xpcf/component/ComponentFactory.h>

namespace org { namespace bcom { namespace xpcf {

/**
 * @class IComponentManager
 * @brief Specifies the IComponentManager interface.
 *
 * This interface provides method definitions to handle component description
 * file loading and component creation.
 * The ComponentManaager is a singleton service.
 * Its instance can be retrieved with the dedicated getComponentManagerInstance() function.
 */
class IComponentManager : virtual public IComponentIntrospect {
public:
    /**
     * Virtual destructor of IComponentManager
     */
    virtual ~IComponentManager() = default;

    virtual SRef<IFactory> getFactory() = 0;
    /**
     * Clear all previously loaded registries.
     * @note created components are not released, and their module counterparts are still loaded in memory.
     */
    virtual void clear() = 0;

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
     * Retrieve every module metadata registered in the ComponentManager
     * @return the collection of modules metadata
     */
    virtual const IEnumerable<SPtr<ModuleMetadata>> & getModulesMetadata() const = 0;

    /**
     * Search a component metadata.
     * @param [in] the component UUID to search for
     * @throws ComponentNotFoundException
     * @return the component metadata for the component UUID provided
     */
    virtual SPtr<ComponentMetadata> findComponentMetadata(const uuids::uuid & componentUUID) const = 0;

    /**
     * Search the module containing the component provided.
     * @param [in] the component UUID
     * @throws ComponentNotFoundException
     * @return the module UUID containing the component provided
     */
    virtual uuids::uuid getModuleUUID(const uuids::uuid & componentUUID) const = 0;

    /**
     * Search a module metadata.
     * @param [in] the module UUID to search for
     * @throws ModuleNotFoundException
     * @return the module metadata for the module UUID provided
     */
    virtual SPtr<ModuleMetadata> findModuleMetadata(const uuids::uuid & moduleUUID) const = 0;

    /**
     * Retrieve every interface metadata registered in the ComponentManager
     * @return the collection of interfaces metadata
     */
    virtual const IEnumerable<SPtr<InterfaceMetadata>> & getInterfacesMetadata() const = 0;

    /**
     * deprecatd ?? TODO : check
     * @param [in]
     * @return
     */
#ifndef SWIG
    [[deprecated]]
#endif
    virtual SPtr<InterfaceMetadata> findInterfaceMetadata(const uuids::uuid & interfaceUUID) const = 0;

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
               uint8_t bindingRangeMask = BindingRange::Default|BindingRange::All > void bindLocal();

    template < typename T, typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit > void bindLocal();

    template < typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Named > void bindLocal(const char * name);

    template < typename T, typename I, typename C, BindingScope scope = BindingScope::Transient,
               uint8_t bindingRangeMask = BindingRange::Explicit > void bindLocal(const char * name);

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
     * @throws InjectableNotFoundException when no bind was found to resolve an instance for I
     * or when the component resolved for I declares injectable(s) and there was missing bind(s) to resolve them
     */
    template < typename I> SRef<I> resolve();

    /**
     *
     * @note with @fn resolve()
     * @param [in]
     * @return
     * @throws InjectableNotFoundException when no bind was found to resolve an instance for {I, name}
     * or when the component resolved for {I, name} declares injectable(s) and there was missing bind(s) to resolve them
     */
    template < typename I> SRef<I> resolve(const char * name);

    /**
     *
     * @note with @fn resolve()
     * @return
     * @throws InjectableNotFoundException when no bind was found to resolve an instance for I
     * or when the component resolved for I declares injectable(s) and there was missing bind(s) to resolve them
     */
    template < typename I> const SRef<IEnumerable<SRef<IComponentIntrospect>>> resolveAll();

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

};

template <> struct InterfaceTraits<IComponentManager>
{
    static constexpr const char * UUID = "F3DBCB05-B8C6-47FB-BF80-E86D97DA46B8";
    static constexpr const char * NAME = "XPCF::IComponentManager";
    static constexpr const char * DESCRIPTION = "Provides component factory facility to create components from modules declared in a registry";
};

template < typename I >
SRef<I> IComponentManager::createComponent(const uuids::uuid& componentUUID)
{
    SRef<IComponentIntrospect> rICIntrospect = createComponent(componentUUID);
    return rICIntrospect->bindTo<I>();
}

template < typename C >
SRef<IComponentIntrospect> IComponentManager::create()
{
    return createComponent(toUUID<C>());
}

template < typename I> SRef<I> IComponentManager::resolve()
{
    return getFactory()->resolve<I>();
}

template < typename I> const SRef<IEnumerable<SRef<IComponentIntrospect>>> IComponentManager::resolveAll()
{
    return getFactory()->resolveAll<I>();
}

template < typename I> SRef<I> IComponentManager::resolve(const char * name)
{
    return getFactory()->resolve<I>(name);
}

template < typename I >
SRef<I> IComponentManager::createComponent(const char * instanceName, const uuids::uuid & componentUUID)
{
    SRef<IComponentIntrospect> rICIntrospect = createComponent(instanceName,componentUUID);
    return rICIntrospect->bindTo<I>();
}

template < typename C >
SRef<IComponentIntrospect> IComponentManager::create(const char * instanceName)
{
    return createComponent(instanceName,toUUID<C>());
}

template < typename I, BindingScope scope,
           uint8_t bindingRangeMask> void  IComponentManager::bind(const uuids::uuid & componentUUID)
{
    getFactory()->bind(toUUID<I>(), componentUUID, scope, bindingRangeMask);
}

template < typename T, typename I, BindingScope scope,
           uint8_t bindingRangeMask> void  IComponentManager::bind(const uuids::uuid & componentUUID)
{
    getFactory()->bind(toUUID<T>(), toUUID<I>(), componentUUID, scope, bindingRangeMask);
}

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void  IComponentManager::bind()
{

    getFactory()->bind<I,C,scope,bindingRangeMask>();
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask > void IComponentManager::bind()
{
    getFactory()->bind<T,I,C,scope,bindingRangeMask>();
}

template < typename I, BindingScope scope,
           uint8_t bindingRangeMask> void  IComponentManager::bind(const char * name, const uuids::uuid & componentUUID)
{
    getFactory()->bind(name, toUUID<I>(), componentUUID, scope, bindingRangeMask);
}

template < typename T, typename I, BindingScope scope,
           uint8_t bindingRangeMask> void  IComponentManager::bind(const char * name, const uuids::uuid & componentUUID)
{
    getFactory()->bind(toUUID<T>(), name, toUUID<I>(), componentUUID, scope);
}

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void  IComponentManager::bind(const char * name)
{
    getFactory()->bind<I,C,scope,bindingRangeMask>(name);
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void  IComponentManager::bind(const char * name)
{
    getFactory()->bind<T,I,C,scope,bindingRangeMask>(name);
}

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IComponentManager::bindLocal()
{
    getFactory()->bindLocal<I,C,scope,bindingRangeMask>();
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IComponentManager::bindLocal()
{
    getFactory()->bindLocal<T,I,C,scope,bindingRangeMask>();
}

template < typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IComponentManager::bindLocal(const char * name)
{
    getFactory()->bindLocal<I,C,scope,bindingRangeMask>(name);
}

template < typename T, typename I, typename C, BindingScope scope,
           uint8_t bindingRangeMask> void IComponentManager::bindLocal(const char * name)
{
    getFactory()->bindLocal<T,I,C,scope,bindingRangeMask>(name);
}

/**
 * Retrieve the component manager instance.
 * @return the smart reference on the ComponentManager interface
 */
XPCF_EXPORT_API SRef<IComponentManager> getComponentManagerInstance();

/**
 * Creates the component identified by componentUUID.
 * @note this method uses the ComponentManager,
 * hence the ComponentManager must be filled first with informations from either an xml or a module with one of load() methods.
 * @param [in] componentUUID the component unique identifier
 * @return the smart reference on the created component
 * @throws InjectableNotFoundException when the component to create declares injectables and no bind exists for an injectable
 */
XPCF_EXPORT_API SRef<IComponentIntrospect> createInstance(const uuids::uuid & componentUUID);

/**
 * Creates the component identified by {instanceName, componentUUID}.
 * @note this method uses the ComponentManager,
 * hence the ComponentManager must be filled first with informations from either an xml or a module with one of load() methods.
 * @param [in] componentUUID the component unique identifier
 * @param [in] instanceName the component instance name to create (used for specific configuration of a component)
 * @return the smart reference on the created component
 * @throws InjectableNotFoundException when the component to create declares injectables and no bind exists for an injectable
 */
XPCF_EXPORT_API SRef<IComponentIntrospect> createInstance(const char * instanceName, const uuids::uuid & componentUUID);

/**
 * Injects @p instance referenced with @p IInjected to @p component
 * @param [in] component the component to which @p instance is injected
 * @param [in] instance the injected component instance
 * @tparam IInjected the injected @p instance interface type
 * @tparam I the @p component interface type
 * @note component must have an injectable declared for @p IInjected
 * @throws InjectionException
 */
template <typename IInjected, typename I = IComponentIntrospect>
void inject(SRef<I> component, SRef<IInjected> instance)
{
    static_assert(utils::is_base_of<IInjectable, I>::value,
                  "Component type passed to inject is not a derived class of IComponentIntrospect !!");
    component->template bindTo<IComponentIntrospect>()->template inject<IInjected>(instance);
}


/**
 * Injects @p instance referenced with { @p IInjected, @p name} to @p component
 * @param [in] component the component to which @p instance is injected
 * @param [in] instance the injected component instance UUID
 * @param [in] name the injectable name
 * @tparam IInjected the injected @p instance interface type
 * @tparam I the @p component interface type
 * @note component must have an injectable declared for { @p IInjected, @p name}
 * @throws InjectionException
 */
template <typename IInjected, typename I = IComponentIntrospect>
void inject(SRef<I> component, SRef<IInjected> instance, const char * name)
{
    static_assert(utils::is_base_of<IComponentIntrospect, I>::value,
                  "Component type passed to inject is not a derived class of IComponentIntrospect !!");
    component->template bindTo<IInjectable>()->template inject<IInjected>(instance, name);
}

/**
* Create a @p C instance for @p IInjected service and inject the created instance to declared @p IInjected injectable for @p component
* @param [in] target the component to which @p instance is injected
* @tparam C the @p component type to create (forward declaration is allowed in this place) -> used to find the ComponentTraits type
 * @tparam IInjected the interface type used to bind the created component
 * @tparam I the @p target interface type
* @note target must have an injectable declared for @p IInjected
* @throws InjectionException
*/
template < typename C, typename IInjected, typename I = IComponentIntrospect >
inline void inject(SRef<I> target)
{
    static_assert(utils::is_base_of<IComponentIntrospect, I>::value,
                  "Component type passed to inject is not a derived class of IComponentIntrospect !!");
    SRef<IInjected> instance = createInstance(toUUID<C>())->template bindTo<IInjected>();
    inject(target, instance);
}

/**
 * Create a @p C instance for @p IInjected service and inject the created instance to declared { @p IInjected, @p name} injectable for @p component
 * @param [in] target the component to which @p instance is injected
 * @param [in] name the injectable name (the name is also used to configure the component)
 * @tparam C the @p component type to create (forward declaration is allowed in this place) -> used to find the ComponentTraits type
 * @tparam IInjected the interface type used to bind the created component
 * @tparam I the @p target interface type
 * @note component must have an injectable declared for { @p IInjected, @p name}
 * @throws InjectionException
 */
template < typename C, typename IInjected, typename I = IComponentIntrospect >
inline void inject(SRef<I> target, const char * name)
{
    static_assert(utils::is_base_of<IComponentIntrospect, I>::value,
                  "Component type passed to inject is not a derived class of IComponentIntrospect !!");
    SRef<IInjected> instance = createInstance(name, toUUID<C>())->template bindTo<IInjected>();
    inject(target, instance, name);
}



}}} //namespace org::bcom::xpcf

#endif
