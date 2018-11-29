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
#include "xpcf/xpcf_api_define.h"

#include <map>
namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API ComponentBase : public virtual IComponentIntrospect
{
    // friend class ComponentFactory;
public:
    ComponentBase() = delete;
    ComponentBase(const uuids::uuid & uuid);
    ComponentBase(std::map<std::string,std::string> componentTrait);
    virtual ~ComponentBase();

    const uuids::uuid & getUUID() const { return m_UUID; }

    // IComponentIntrospect
    uint32_t          getNbInterfaces() const final;
    const IEnumerable<uuids::uuid> & getInterfaces() const final;

    const char*       getDescription(const uuids::uuid& interfaceUUID) const final;
    InterfaceMetadata getMetadata(const uuids::uuid& interfaceUUID) const final;

    // It is the component that decides if it must or must not unload when all
    // refs are out
    virtual void unloadComponent() = 0;


protected:
    template <class T> void addInterface(T* componentThis);


private:
    SRef<IComponentIntrospect> introspect() final;
    void addComponentRef() final;
    void releaseComponentRef() final;
    void addInterface(uuids::uuid& interfaceUUID, utils::any componentThis, const char * name, const char * description);

    utils::any queryInterface(const uuids::uuid& interfaceUUID) const final;
    bool implements(const uuids::uuid& interfaceUUID) const final;

    class InternalImpl;
    UniqueRef<InternalImpl> m_pimpl;

    std::map<std::string,std::string> m_componentTrait;

    const uuids::uuid m_UUID;
    uint32_t m_usageRefCount;
};

template <class T>
void ComponentBase::addInterface(T* componentThis)
{
    static_assert(is_interface<T>::value,
                  "Type passed to addInterface is not an interface "
                  "or InterfaceTraits not defined !!");

    static_assert(utils::is_base_of<IComponentIntrospect, T>::value,
                  "Interface type passed to addInterface is not a derived class of IComponentIntrospect !!");

    uuids::uuid interfaceId = toUUID<T>();
    addInterface(interfaceId, componentThis, InterfaceTraits<T>::NAME, InterfaceTraits<T>::DESCRIPTION);
}

}}} //namespace org::bcom::xpcf

#endif
