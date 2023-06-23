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
 * @date 2019-05-27
 */

#ifndef ORG_BCOM_XPCF_IINJECTABLE_H
#define ORG_BCOM_XPCF_IINJECTABLE_H

#include <xpcf/api/IComponentIntrospect.h>
#include <xpcf/api/InjectableMetadata.h>
#include <xpcf/core/uuid.h>
#include <xpcf/core/XPCFErrorCode.h>
#include <xpcf/collection/ICollection.h>
#include <string>

namespace org { namespace bcom { namespace xpcf {

/**
 * @class IInjectable
 * @brief Specifies the IInjectable interface.
 *
 * This interface provides method definitions to handle XPCF component injection.
 * Every component inheriting ComponentBase implicitly implements this interface.
 *
 */
class IInjectable : virtual public IComponentIntrospect {
public:
    virtual ~IInjectable() = default;

    /**
     * Retrieve the enumerator upon injectables interface UUIDs' the underlying component expects.
     * @return a unique pointer on the UUIDs' enumerator
     */
    virtual const IEnumerable<SPtr<Injector>> & getInjectables() const = 0;

    /**
     * Inject concrete @em instance to service @em I
     * @throws InjectionException when the underlying component doesn't have an injectable for @em I
     */
    template <typename I> void inject(SRef<I> instance);

    template <typename I> void inject(SRef<IEnumerable<SRef<IComponentIntrospect>>> & instance);

    /**
     * Bind concrete instance @em instance from IComponentIntrospect to service @em I and inject instance to I
     * @throws InjectionException when the underlying component doesn't have an injectable for @em I
     */
    template <typename I> void inject(SRef<IComponentIntrospect> instance);

    /**
     * Bind concrete instance @em instance to service @em I and inject instance to injectable {name, I}
     * @throws InjectionException when the underlying component doesn't have an injectable for @em {I, name}
     */
    template <typename I> void inject(SRef<I> instance, const char * name);

    /**
     * Bind concrete instance @em instance from IComponentIntrospect to service @em I and inject instance to injectable {name, I}
     * @throws InjectionException when the underlying component doesn't have an injectable for @em {I, name}
     */
    template <typename I>
    void inject(SRef<IComponentIntrospect> instance, const char * name);

    template <typename I>
    bool injectExists() const;

    template <typename I>
    bool injectExists(const char * name) const;

    virtual bool injectExists(const uuids::uuid & interfaceUUID) const = 0;

    virtual bool injectExists(const uuids::uuid & interfaceUUID, const char * name) const = 0;

    /**
     * onInjected is called after injection step has occurred.
     * It can be overriden in concrete components to add post-injection code,
     * for instance code that relies on xml properties.
     * @return XPCFErrorCode::_SUCCESS upon successfull completion, one of the other
     * XPCFErrorCode error otherwise (it's up to the component implementation to return appropriate error code).
     */
    virtual void onInjected() = 0;

protected:
    /**
     * Retrieve the unnamed injectable referenced with @em interfaceUUID
     * @return an any holding the address of the injectable
     * @throws InjectionException when the underlying component doesn't have an injectable for @em interfaceUUID
     */
    virtual utils::any retrieveInjectable(const uuids::uuid & interfaceUUID) const = 0;

    /**
     * Retrieve the named injectable referenced with @em [name, interfaceUUID]
     * @return an any holding the address of the injectable
     * @throws InjectionException when the underlying component doesn't have an injectable for @em {interfaceUUID, name}
     */
    virtual utils::any retrieveInjectable(const uuids::uuid & interfaceUUID, const char * name) const = 0;

    virtual utils::any retrieveMultiInjectable(const uuids::uuid & interfaceUUID) = 0;
};

template <typename I>
void IInjectable::inject(SRef<I> instance)
{
    static_assert(utils::is_base_of<IComponentIntrospect, I>::value,
                  "Interface type passed to createComponent is not a derived class of IComponentIntrospect !!");
    utils::any obj = retrieveInjectable(toUUID<I>());
    SRef<I> * injectable = utils::any_cast<SRef<I>*>(obj);
    *injectable = instance;
}

template <typename I>
void IInjectable::inject(SRef<IComponentIntrospect> instance)
{
    inject<I>(instance->bindTo<I>());
}

template <typename I>
void IInjectable::inject(SRef<IEnumerable<SRef<IComponentIntrospect>>> & instanceCollection)
{
    static_assert(utils::is_base_of<IComponentIntrospect, I>::value,
                  "Interface type passed to createComponent is not a derived class of IComponentIntrospect !!");
    utils::any objSet =  retrieveMultiInjectable(toUUID<I>());
    SRef<ICollection<SRef<I>>> injectableSet = utils::any_cast<SRef<ICollection<SRef<I>>>>(objSet);
    if (injectableSet->size() > instanceCollection->size()) {
        //throw
        return;
    }

    for (auto instance : *instanceCollection) {
        injectableSet->add(instance->bindTo<I>());
    }
}

template <typename I>
void IInjectable::inject(SRef<I> instance, const char * name)
{
    static_assert(utils::is_base_of<IComponentIntrospect, I>::value,
                  "Interface type passed to createComponent is not a derived class of IComponentIntrospect !!");
    utils::any obj = retrieveInjectable(toUUID<I>(), name);
    SRef<I> * injectable = utils::any_cast<SRef<I>*>(obj);
    *injectable = instance;
}

template <typename I>
void IInjectable::inject(SRef<IComponentIntrospect> instance, const char * name)
{
    inject<I>(instance->bindTo<I>(), name);
}

template <typename I>
bool IInjectable::injectExists() const
{
   return injectExists(toUUID<I>());
}

template <typename I>
bool IInjectable::injectExists(const char * name) const
{
    return injectExists(toUUID<I>(), name);
}

template <> struct InterfaceTraits<IInjectable>
{
    static constexpr const char * UUID = "6C5B9EDC-C78B-47FD-8CB2-2BF8D7EDE4EA";
    static constexpr const char * NAME = "XPCF::IInjectable";
    static constexpr const char * DESCRIPTION = "Provides dependency injection interface";
};

}}}

#endif // ORG_BCOM_XPCF_IINJECTABLE_H
