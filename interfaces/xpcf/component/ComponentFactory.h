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

#ifndef ORG_BCOM_XPCF_COMPONENTFACTORY_H
#define ORG_BCOM_XPCF_COMPONENTFACTORY_H

#include "xpcf/api/IComponentIntrospect.h"
// NOTE : ALL COMPONENTS MUST THROW EXCEPTIONS DERIVED from std::exception,
// in order to be able to catch all exceptions in a uniform way

namespace org { namespace bcom { namespace xpcf {

// Other shared rI counting solution :
// Add virtual release method upon interface,
// for each shared returned provide this method as custom deleter
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
     * Creates a component of type C. C must be an IComponentIntrospect derived class.
     * C must meet the is_class requirement from std::type_traits library.
     * @param [inout] an empty IComponentIntrospect smart reference that will be initialized
     * with the component created
     * @throws
     * @return             FAILS when T doesn't fulfill req ?
     */
    template <class C> static SRef<IComponentIntrospect> create();

    /**
     * Delegates a T component creation.
     * This method MUST BE specialized for the concrete T component type - i.e. it must be specialized
     * in the component implementation.
     * This mechanism insures that component allocation takes place in the same translation unit where
     * deallocation will occur.
     * @return a pointer on the component created
     */
    template <class T> static T* createInstance();
};

//can throw std::bad_alloc
template <class C>
SRef<IComponentIntrospect> ComponentFactory::create()
{
    static_assert(utils::is_base_of<IComponentIntrospect, C>::value,
                  "Type passed to createComponent is not a derived class of IComponentIntrospect !!");

    C* component = ComponentFactory::createInstance<C>();
    return IComponentIntrospect::acquireComponentRef<C, IComponentIntrospect>(component);
}

#if _WIN32
// XPCF_MODULEHOOKS_API is always defined as __declspec(dllexport) :
// the macro is included as part of the client module,
// those methods are not intended to be imported at link phase,
// only dynamically binded through dlopen/LoadLibrary mechanisms
#define XPCF_MODULEHOOKS_API __declspec(dllexport)
#else
#define XPCF_MODULEHOOKS_API
#endif

/**
 * Provides a helper to define the UUID symbol of a component.
 * This macro must be used inside the component body (i.e. the .cpp file), to insure the respect
 * of the ODR rule.
 *
 * @param  fullComponentType the full component type, for instance namespace::componentClass
 */
#define XPCF_DEFINE_UUID_SYMBOL(fullComponentType) //constexpr const char * fullComponentType::UUID

/**
 * Default definition for ComponentFactory::createInstance.
 * This definition provides the dynamic creation of the component instance.
 * If the component must provide a different strategy, specialize the
 * ComponentFactory::createInstance method and override ComponentBase::unloadComponent
 * directly inside the component body
 * This macro must be used inside the component body (i.e. the .cpp file), to insure the respect
 * of the ODR rule.
 * Moreover the creation must occur inside the translation unit of the component.
 *
 * @param  fullComponentType the full component type, for instance namespace::componentClass
 * @return the pointer on the new object created
 */
#define XPCF_DEFINE_FACTORY_CREATE_INSTANCE(fullComponentType) \
namespace org { namespace bcom { namespace xpcf { \
template<> XPCF_MODULEHOOKS_API fullComponentType* ComponentFactory::createInstance() { \
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
