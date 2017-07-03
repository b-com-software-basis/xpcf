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

#ifndef __ComponentBase_H__
#define __ComponentBase_H__

#include <boost/uuid/uuid.hpp>
#include <map>
#include <iostream>
#include "ComponentFactory.h"

namespace org {
namespace bcom {
namespace xpcf {

class XPCF_EXPORT_API ComponentBase : public virtual IComponentIntrospect
{
     friend class ComponentFactory;
public:
    ComponentBase();
    virtual ~ComponentBase();
    const uuids::uuid & getUUID() const { return m_UUID; }

    // IComponentIntrospect
    //
    int               getNbInterfaces() const final;
    void              getInterfaces(uuids::uuid* interfacesArray) const final;
    const char*       getDescription(const uuids::uuid& interfaceUUID) const final;

    // It is the component that decides if it must or must not unload when all
    // refs are out
    virtual void unloadComponent() = 0;

protected :
    template <class T>
    void addInterface(T* componentThis, const char * interfaceID,  const char * name);
    void setUUID(const uuids::uuid& componentUUID);
    void setUUID(const char * componentUUID);

private:
    SRef<IComponentIntrospect> introspect() final;
    void addComponentRef() final;
    void releaseComponentRef() final;
    void addInterface(uuids::uuid& interfaceUUID, utils::any componentThis, const char * name);

    utils::any queryInterface(const uuids::uuid& interfaceUUID) const final;

    class InternalImpl;
    SRef<InternalImpl> m_internalImpl;

    uuids::uuid m_UUID;
    unsigned long m_count;
};

template <class T>
void ComponentBase::addInterface(T* componentThis, const char * interfaceUUID, const char * name)
{
    if (utils::is_base_of<IComponentIntrospect,T>::value) {
         uuids::uuid interfaceId = toUUID(interfaceUUID);
         addInterface(interfaceId, componentThis,name);
    }
    else {
        //TODO :handle error
    }
}

}}} //namespace org::bcom::xpcf

#endif
