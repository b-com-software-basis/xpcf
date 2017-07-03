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

#ifndef __IComponentManager_H__
#define __IComponentManager_H__

#include "XPCF_definitions.h"
#include "ComponentMetadata.h"
#include "ContainerMetadata.h"
#include "InterfaceMetadata.h"
#include "IComponentIntrospect.h"

#include <boost/uuid/uuid.hpp>
#include <boost/filesystem.hpp>

#include <vector>

namespace org {
namespace bcom {
namespace xpcf {

//TODO :
// - multi configuration files support : implemented TO TEST !!(now isLoaded is set once a first configuration file is successfully loaded, but doesn't prevent from loading other configuration files)
// - configuration file generation from : 1/ a container 2/ a path to search containers for

class IComponentManager : public virtual IComponentIntrospect {
public:
  virtual ~IComponentManager() = default;
  virtual unsigned long load() = 0;
  virtual unsigned long load(const char* libraryFilePath) = 0;

  virtual bool isLoaded() const = 0;

  //virtual SPtr<tinyxml2::XMLDocument> introspectContainer(const char* containerFilePath) = 0;
  virtual SPtr<ContainerMetadata> introspectContainer(const char* containerFilePath) = 0;
  virtual SPtr<ContainerMetadata> getContainerComponentList(const uuids::uuid& componentUUID) = 0;
  virtual void addContainerRef(const uuids::uuid& containerUUID) = 0;
  virtual void releaseContainerRef(const uuids::uuid& containerUUID) = 0;

  template <class T>
  unsigned long createComponent(const uuids::uuid& componentUUID,const uuids::uuid& interfaceUUID, SRef<T> & componentRef);
  virtual unsigned long createComponent(const uuids::uuid& componentUUID, SRef<IComponentIntrospect> & componentRef) = 0;

  virtual int getNbComponentMetadatas() const = 0;
  virtual SPtr<ComponentMetadata> getComponentMetadata(int) const = 0;
  virtual SPtr<ComponentMetadata> findComponentMetadata(const uuids::uuid &) const = 0;

  virtual int getNbInterfaceMetadatas() const = 0;
  virtual SPtr<InterfaceMetadata> getInterfaceMetadata(int) const = 0;
  virtual SPtr<InterfaceMetadata> findInterfaceMetadata(const uuids::uuid&) const = 0;

  static constexpr const char * UUID = "F3DBCB05-B8C6-47FB-BF80-E86D97DA46B8";

};

template <class T>
unsigned long IComponentManager::createComponent(const uuids::uuid& componentUUID,const uuids::uuid& interfaceUUID,SRef<T>& componentRef)
{
    SRef<IComponentIntrospect> rICIntrospect;
    unsigned long result = createComponent(componentUUID, rICIntrospect);
    if (result != XPCF_OK) {
        return result;
    }
    rICIntrospect->queryInterface(interfaceUUID, componentRef);
    return XPCF_OK;
}


XPCF_EXPORT_API SRef<IComponentManager> getComponentManagerInstance();

}}} //namespace org::bcom::xpcf

#endif
