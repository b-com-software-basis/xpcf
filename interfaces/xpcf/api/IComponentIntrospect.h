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

#ifndef ORG_BCOM_XPCF_ICOMPONENTINTROSPECT_H
#define ORG_BCOM_XPCF_ICOMPONENTINTROSPECT_H

#include "xpcf/collection/IEnumerable.h"
#include "xpcf/core/traits.h"
#include "xpcf/api/InterfaceMetadata.h"

// NOTE : ALL COMPONENTS MUST THROW EXCEPTIONS DERIVED from std::exception,
// in order to be able to catch all exceptions in a uniform way
//TODO : namespacing normalization
namespace org { namespace bcom { namespace xpcf {

/**
  * @class IComponentIntrospect
  * @brief Specifies the IComponentIntrospect interface.
  *
  * This interface provides method definitions to handle component introspection.
  * The ComponentFactory class is a friend class to allow access to protected
  * and private methods of IComponentIntrospect
  */
class IComponentIntrospect
{
    friend class ComponentFactory;
public:
    /**
     * Virtual destructor of IComponentIntrospect
     */
    virtual ~IComponentIntrospect() = default;

    /**
     * Returns the number of interfaces the component implements
     * @return the number of interfaces
     */
    virtual uint32_t getNbInterfaces() const = 0;

    /**
     * Retrieve the enumerator upon interface UUIDs' implemented by the underlying component.
     * @return a unique pointer on the UUIDs' enumerator
     */
    virtual const IEnumerable<uuids::uuid> & getInterfaces() const = 0;

    /**
     * Runtime checks whether the component implements an interface
     * @param [in] interfaceUUID the interface unique ID
     * @return true when the component implements the interface @em interfaceUUID, false otherwise.
     */
    virtual bool implements(const uuids::uuid& interfaceUUID) const = 0;

    /**
     * Retrieves an interface description from its UUID
     * @param [in] interfaceUUID the interface unique ID
     * @return               the interface description
     */
    virtual const char* getDescription(const uuids::uuid& interfaceUUID) const = 0;

    /**
     * Retrieves an interface metadata from its UUID
     * @param [in] interfaceUUID the interface unique ID
     * @throws  InterfaceNotImplementedException when there is no interface metadata for @em interfaceUUID
     * @return  the interface metadata
     */
    virtual const InterfaceMetadata & getMetadata(const uuids::uuid& interfaceUUID) const = 0;

    /**
     * Queries the interface referenced with @em interfaceUUID and returns a reference to the interface I.
     * @param  interfaceUUID [description]
     * @param  aRef          [description]
     * @throws  InterfaceNotImplementedException when the component doesn't implement interface @em interfaceUUID
     * @throws  utils::bad_any_cast when @em interfaceUUID requested is not the UUID of interface @em I
     * @return the shared reference upon the requested interface @em I
     */

    template <class I>
    SRef<I> queryInterface(const uuids::uuid& interfaceUUID);

    /**
     * Binds the component to the requested interface.
     * This method uses the InterfaceTraits defined for I to retrieve the interface UUID.
     * @throws  InterfaceNotImplementedException when the component doesn't implement interface @em interfaceUUID
     * @throws  utils::bad_any_cast when @em interfaceUUID requested is not the UUID of interface @em I
     * @return the shared reference upon the requested interface @em I
     */
    template <class I>
    SRef<I> bindTo();

    template <class I>
    bool implements() const;

protected:
    /**
     * Retrieve a component or interface smart reference from its pointer,
     * incrementing the component reference counter with @fn addComponentRef.
     * The smart reference created is initialized with the
     * IComponentIntrospectDeleter custom deleter.
     * This deleter calls @fn releaseComponentRef when the smart reference is destroyed.
     * @param component the component or interface pointer
     * @param aRef      the component smart reference
     */
    template <class T, class B> static void acquireComponentRef(T* component, SRef<B>& aRef);

    template <class T, class B> static SRef<B> acquireComponentRef(T* component);

    /**
   * [queryInterface description]
   * @param  interfaceUUID [description]
   * @return               [description]
   */
    virtual utils::any queryInterface(const uuids::uuid & interfaceUUID) const = 0;

private:
    /**
     * @return the smart reference on the IComponentIntrospect interface of the underlying component
     */
    virtual SRef<IComponentIntrospect> introspect() = 0;

    /**
   * Increment the reference counter of the component
   */
    virtual void addComponentRef() = 0;

    /**
   * Decrement the reference counter of the component
   */
    virtual void releaseComponentRef() = 0;

    template <class T>
    class IComponentIntrospectDeleter
    {
    public:
        void operator()(T * p)
        {
            if (p) {
                static_cast<IComponentIntrospect*>(p)->releaseComponentRef();
            }
        }
    };
};

template <> struct InterfaceTraits<IComponentIntrospect>
{
    static constexpr const char * UUID = "125f2007-1bf9-421d-9367-fbdc1210d006";
    static constexpr const char * NAME = "XPCF::IComponentIntrospect";
    static constexpr const char * DESCRIPTION = "Component introspection interface.\nProvides:\n- browsing of component interfaces'\n- component binding to its implemented interfaces";
};


template <class T, class B> void IComponentIntrospect::acquireComponentRef(T* component,
                                                                           SRef<B>& componentRef)
{
    componentRef = SRef<B>(component, IComponentIntrospectDeleter<T>());
    static_cast< SRef<IComponentIntrospect> >(componentRef)->addComponentRef();
}

template <class T, class B> SRef<B> IComponentIntrospect::acquireComponentRef(T* component)
{
    SRef<B> componentRef = SRef<B>(component, IComponentIntrospectDeleter<T>());
    static_cast< SRef<IComponentIntrospect> >(componentRef)->addComponentRef();
    return componentRef;
}

template <class I>
SRef<I> IComponentIntrospect::queryInterface(const uuids::uuid& interfaceUUID)
{
    static_assert(utils::is_base_of<IComponentIntrospect, I>::value,
                  "Interface type passed to createComponent is not a derived class of IComponentIntrospect !!");
    SRef<I> componentRef;
    utils::any obj = queryInterface(interfaceUUID);
    componentRef = IComponentIntrospect::acquireComponentRef<I, I>(utils::any_cast<I*>(obj));
    return componentRef;
}

template <class I>
SRef<I> IComponentIntrospect::bindTo()
{
    static_assert(is_interface<I>::value,
                  "Type I passed to bindTo<I> is not an interface"
                  "or InterfaceTraits is not defined for I !!");
    return queryInterface<I>(toUUID<I>());
}

template <class I>
bool IComponentIntrospect::implements() const
{
    return implements(toUUID<I>());
}

template <class I>
SRef<I> bindTo(SRef<IComponentIntrospect> component)
{
    return component->bindTo<I>();
};

}}} //namespace org::bcom::xpcf

#endif
