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

#ifndef __ComponentFactory_H__
#define __ComponentFactory_H__

#include <iostream>
#include "IComponentIntrospect.h"
// NOTE : ALL COMPONENTS MUST THROW EXCEPTIONS DERIVED from std::exception, in order to be able to catch all exceptions in a uniform way
// OR NO exceptions ?

namespace org {
namespace bcom {
namespace xpcf {

// Other shared rI counting solution :
// Add virtual release method upon interface, for each shared returned provide this method as custom deleter
// Increment count for each shared (rI or concrete) returned
// decrement count foreach release call
// inside concrete release impl, delete this when count == 0 from Unload()

/** @class ComponentFactory
* @brief Specifies the ComponentFactory class.
*
* This class provides factory method definitions to handle component creation.
* The ComponentFactory class is a friend class of IComponentIntrospect interface
 */
class ComponentFactory {
public:
    /**
     * Creates a component of type T. T must be an IComponentIntrospect derived class. T must meet the is_class requirement from std::type_traits library.
     * @param [inout] an empty IComponentIntrospect smart reference that will be initialized with the component created
     * @return             FAILS when T doesn't fulfill req ?
     */
    template <class T> static void createComponent(SRef<IComponentIntrospect> & componentRef);

    /**
     * Creates a component of type T and retrieve the component as a smart reference to interface U.
     * T must be an U derived class. U must be an IComponentIntrospect derived class.
     * T and U must meet the is_class requirement from std::type_traits library.
     * @param [inout] an empty U smart reference that will be initialized with the component created
     * @return             FAILS when T doesn't fulfill req ?
     */
    template <class T,class U> static void createComponent(const uuids::uuid& interfaceUUID, SRef<U> & componentRef);

    template <class T,class U> static void createComponent(const char * interfaceUUID, SRef<U> & componentRef);

    // The method below MUST BE specialized in the component implementation
    //

/**
 * Delegates a T component creation.
 * This method MUST BE specialized for the concrete T component type - i.e. it must be specialized in the component implementation.
 * This mechanism insures that component allocation takes place in the same translation unit where deallocation will occur.
 * @return a pointer on the component created
 */
    template <class T> static T* createInstance();
};

template <class T> void ComponentFactory::createComponent(SRef<IComponentIntrospect> & componentRef)
{
    if (utils::is_base_of<IComponentIntrospect,T>::value) {
        T* component = ComponentFactory::createInstance<T>();
        componentRef = IComponentIntrospect::acquireComponentRef<T,IComponentIntrospect>(component);
    }
}

template <class T, class U> void ComponentFactory::createComponent(const char * interfaceUUID, SRef<U> & componentRef)
{
    createComponent(toUUID(interfaceUUID), componentRef);
}
template <class T, class U> void ComponentFactory::createComponent(const uuids::uuid& interfaceUUID, SRef<U> & componentRef)
{
    if ((utils::is_base_of<IComponentIntrospect,U>::value) && (utils::is_base_of<U,T>::value)) {
        SRef<IComponentIntrospect> iRef;
        createComponent<T>(iRef);
        iRef->queryInterface(interfaceUUID,componentRef);
    }
}


/**
 * Provides a helper to define the UUID symbol of a component.
 * This macro must be used inside the component body (i.e. the .cpp file), to insure the respect of the ODR rule.
 *
 * @param  fullComponentType the full component type, for instance namespace::componentClass
 */
#define XPCF_DEFINE_UUID_SYMBOL(fullComponentType) constexpr const char * fullComponentType::UUID

/**
 * Default definition for ComponentFactory::createInstance.
 * This definition provides the dynamic creation of the component instance.
 * If the component must provide a different strategy, specialize the ComponentFactory::createInstance method and override ComponentBase::unloadComponent directly inside the component body
 * This macro must be used inside the component body (i.e. the .cpp file), to insure the respect of the ODR rule.
 * Moreover the creation must occur inside the translation unit of the component.
 *
 * @param  fullComponentType the full component type, for instance namespace::componentClass
 * @return the pointer on the new object created
 */
#define XPCF_DEFINE_FACTORY_CREATE_INSTANCE(fullComponentType) \
namespace org { namespace bcom { namespace xpcf { \
template<> XPCF_EXPORT_API fullComponentType* ComponentFactory::createInstance() { \
    return new fullComponentType(); \
}\
}}} \
void fullComponentType::unloadComponent () \
{\
    delete this; \
}\
XPCF_DEFINE_UUID_SYMBOL(fullComponentType)

}}} //namespace org::bcom::xpcf

#endif
