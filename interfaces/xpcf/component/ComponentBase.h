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

#ifndef ORG_BCOM_XPCF_COMPONENTBASE_H
#define ORG_BCOM_XPCF_COMPONENTBASE_H

#include "xpcf/component/ComponentFactory.h"
#include "xpcf/api/IInjectable.h"
#include "xpcf/xpcf_api_define.h"
#include <functional>
#include <map>
using std::placeholders::_1;
namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API ComponentBase : public virtual IInjectable
{
    // friend class ComponentFactory;
public:
    ComponentBase() = delete;
    ComponentBase(const uuids::uuid & uuid);
    ComponentBase(std::map<std::string,std::string> componentTrait);
    virtual ~ComponentBase() override;

    const uuids::uuid & getUUID() const { return m_UUID; }

    // IComponentIntrospect
    uint32_t getNbInterfaces() const final;
    const IEnumerable<uuids::uuid> & getInterfaces() const final;
    //IInjectable
    const IEnumerable<SPtr<Injector>> & getInjectables() const override;

    const char* getDescription(const uuids::uuid& interfaceUUID) const final;
    const InterfaceMetadata & getMetadata(const uuids::uuid& interfaceUUID) const final;

    inline virtual void onInjected() override {}
    // It is the component that decides if it must or must not unload when all
    // refs are out
    virtual void unloadComponent() = 0;


protected:
    template <class T> [[deprecated]] void addInterface(T* componentThis);

    /**
     *
     * @param [in]
     * @param [in]
     * @return
     */
    template <class T> void declareInterface(T* componentThis);

    /**
     *
     * @param [in]
     * @param [in]
     * @return
     * @throws InjectableDeclarationException when another injectable has already been declared for I
     */
    template <typename I> void declareInjectable(SRef<I> & injectable, bool optional = false);

    /**
     *
     * @param [in]
     * @param [in]
     * @return
     * @throws InjectableDeclarationException when another injectable has already been declared for {I, name}
     */
    template <typename I> void declareInjectable(SRef<I> & injectable, const char * name, bool optional = false);

private:
    SRef<IComponentIntrospect> introspect() final;
    void addComponentRef() final;
    void releaseComponentRef() final;
    void declareInterface(const uuids::uuid& interfaceUUID, utils::any componentThis, const char * name, const char * description);
    void declareInjectable(const uuids::uuid& interfaceUUID, utils::any injectable, const std::function<void(SRef<IComponentIntrospect>)> & injector, const char * instanceName, bool optional = false);
    void declareInjectable(const uuids::uuid& interfaceUUID, utils::any injectable, const std::function<void(SRef<IComponentIntrospect>)> & injector, bool optional = false);
    utils::any queryInterface(const uuids::uuid& interfaceUUID) const final;
    utils::any retrieveInjectable(const uuids::uuid & interfaceUUID) const final;
    utils::any retrieveInjectable(const uuids::uuid &  interfaceUUID, const char * instanceName) const final;
    bool injectExists(const uuids::uuid & interfaceUUID) const final;
    bool injectExists(const uuids::uuid & interfaceUUID, const char * name) const final;

    bool implements(const uuids::uuid& interfaceUUID) const final;

    class InternalImpl;
    UniqueRef<InternalImpl> m_pimpl;

    std::map<std::string,std::string> m_componentTrait;

    const uuids::uuid m_UUID;
    uint32_t m_usageRefCount;
};


template <class T>
void ComponentBase::declareInterface(T* componentThis)
{
    static_assert(is_interface<T>::value,
                  "Type passed to addInterface is not an interface "
                  "or InterfaceTraits not defined !!");

    static_assert(utils::is_base_of<IComponentIntrospect, T>::value,
                  "Interface type passed to addInterface is not a derived class of IComponentIntrospect !!");

    uuids::uuid interfaceId = toUUID<T>();
    declareInterface(interfaceId, componentThis, InterfaceTraits<T>::NAME, InterfaceTraits<T>::DESCRIPTION);
}

template <class T>
void ComponentBase::addInterface(T* componentThis)
{
    declareInterface<T>(componentThis);
}


template <typename I>
void ComponentBase::declareInjectable(SRef<I> & injectable, bool optional)
{
    static_assert(is_interface<I>::value,
                  "Type passed to declareInjectable is not an interface "
                  "or InterfaceTraits not defined !!");

    static_assert(utils::is_base_of<IComponentIntrospect, I>::value,
                  "Interface type passed to declareInjectable is not a derived class of IComponentIntrospect !!");
    uuids::uuid interfaceId = toUUID<I>();
    std::function<void(SRef<IComponentIntrospect>)> injector = [this] (SRef<IComponentIntrospect> instance) { inject<I>(instance); };
    declareInjectable(interfaceId, &injectable, injector, optional);
}

template <typename I>
void ComponentBase::declareInjectable(SRef<I> & injectable, const char * instanceName, bool optional)
{
    static_assert(is_interface<I>::value,
                  "Type passed to declareInjectable is not an interface "
                  "or InterfaceTraits not defined !!");

    static_assert(utils::is_base_of<IComponentIntrospect, I>::value,
                  "Interface type passed to declareInjectable is not a derived class of IComponentIntrospect !!");
    uuids::uuid interfaceId = toUUID<I>();
    const std::string name(instanceName);
    std::function<void(SRef<IComponentIntrospect>)> injector = [this, name] (SRef<IComponentIntrospect> instance) {
        inject<I>(instance, name.c_str()); };
    declareInjectable(interfaceId, &injectable, injector, instanceName, optional);
}

}}} //namespace org::bcom::xpcf

#endif
