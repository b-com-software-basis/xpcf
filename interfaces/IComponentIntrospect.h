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

#ifndef __IComponentIntrospect_H__
#define __IComponentIntrospect_H__

#include "XPCF_definitions.h"
#include <iostream>

// NOTE : ALL COMPONENTS MUST THROW EXCEPTIONS DERIVED from std::exception, in order to be able to catch all exceptions in a uniform way
//TODO : namespacing normalization
namespace org {
namespace bcom {
namespace xpcf {
/*$
  * @class IComponentIntrospect
  * @brief Specifies the IComponentIntrospect interface.
  *
  * This interface provides method definitions to handle component introspection.
  * The ComponentFactory class is a friend class to allow access to protected and private methods of IComponentIntrospect
  */
class IComponentIntrospect {
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
  virtual int               getNbInterfaces() const = 0;

  /**
   * Fills the array interfacesArray with the interface UUIDs' list.
   * @note The array must be allocated on the user side. It must be allocated with the number of interfaces retrieved with @fn getNbInterfaces()
   * @param [in] interfacesArray
   * @return                NULLobject if interfacesArray is null
   */
  virtual void              getInterfaces(uuids::uuid* interfacesArray) const = 0;

/**
 * Retrieves an interface description from its UUID
 * @param [in] interfaceUUID the interface unique ID
 * @return               the interface description
 */
  virtual const char*       getDescription(const uuids::uuid& interfaceUUID) const = 0;

  // Naming :  queryInterface or bindTo ?
/**
 * [queryInterface description]
 * @param  interfaceUUID [description]
 * @param  aRef          [description]
 * @return               [description]
 */
  template <class T>
  unsigned long     queryInterface(const uuids::uuid& interfaceUUID, SRef<T>& aRef);

  template <class T>
  SRef<T>    queryInterface(const uuids::uuid& interfaceUUID);

  static constexpr const char * UUID = "125f2007-1bf9-421d-9367-fbdc1210d006";

protected:
    /**
     * Retrieve a component or interface smart reference from its pointer, incrementing the component reference counter with @fn addComponentRef.
     * The smart reference created is initialized with the IComponentIntrospectDeleter custom deleter.
     * This deleter calls @fn releaseComponentRef when the smart reference is destroyed.
     * @param component the component or interface pointer
     * @param aRef      the component smart reference
     */
  template <class T, class B> static void acquireComponentRef(T* component,SRef<B>& aRef);

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
  virtual SRef<IComponentIntrospect>    introspect() = 0;

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

template <class T, class B> void IComponentIntrospect::acquireComponentRef(T* component,SRef<B>& componentRef) {
    componentRef = SRef<B>(component,IComponentIntrospectDeleter<T>());
    static_cast<SRef<IComponentIntrospect>>(componentRef)->addComponentRef();
}

template <class T, class B> SRef<B> IComponentIntrospect::acquireComponentRef(T* component) {
    SRef<B> componentRef = SRef<B>(component,IComponentIntrospectDeleter<T>());
    static_cast<SRef<IComponentIntrospect>>(componentRef)->addComponentRef();
    return componentRef;
}


template <class T>
unsigned long     IComponentIntrospect::queryInterface(const uuids::uuid& interfaceUUID, SRef<T>& aRef)
{
    aRef =  queryInterface<T>(interfaceUUID);

    if (!aRef) {
        return XPCF_FAIL;
    }

    return XPCF_OK;
}

template <class T>
SRef<T> IComponentIntrospect::queryInterface(const uuids::uuid& interfaceUUID)
{
    SRef<T> componentRef;
    utils::any obj = queryInterface(interfaceUUID);
    if (obj.empty()) {
        return componentRef;
    }

    //Use a custom deleter and component ref counter
    try {
        //IComponentIntrospect::acquireComponentRef(utils::any_cast<T*>(obj),componentRef);
        componentRef = IComponentIntrospect::acquireComponentRef<T,T>(utils::any_cast<T*>(obj));
    }
    catch (const utils::bad_any_cast &) {
    // continue to function end : reference will be empty
    }
    return componentRef;
}

inline uuids::uuid toUUID(const char * uuidString)
{
    uuids::string_generator gen;
    return gen(uuidString);
}

/**
 * [XPCF_DECLARE_UUID description]
 * @param  uuid [description]
 * @return      [description]
 */
#define XPCF_DECLARE_UUID(uuid) static constexpr const char * UUID = uuid

}}} //namespace org::bcom::xpcf

#endif
