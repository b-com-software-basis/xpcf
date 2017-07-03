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
 * @date 2017-08-18
 */

#ifndef __ComponentManager_H__
#define __ComponentManager_H__

//#define BOOST_ALL_DYN_LINK 1
#include "XPCF_definitions.h"
#include "ComponentMetadata.h"
#include "InterfaceMetadata.h"
#include "IComponentIntrospect.h"
#include "IComponentManager.h"
#include "ComponentBase.h"

/*#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>*/
#include <boost/filesystem.hpp>


#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

namespace org {
namespace bcom {
namespace xpcf {
//ComponentLibrary or ContainerManager ?
class XPCF_EXPORT_API ComponentManager : public ComponentBase,
                                         public IComponentManager {
public:
  static ComponentManager* instance();
  unsigned long load();
  unsigned long load(const char* libraryFilePath);
  bool isLoaded() const;
  unsigned long createComponent(const uuids::uuid& componentUUID, SRef<IComponentIntrospect> & componentRef) final;

  void unloadComponent ();
 // SRef<T> createComponent(uuids::uuid& componentUUID,uuids::uuid& riid);
  SPtr<ContainerMetadata> introspectContainer(const char* containerFilePath);
  SPtr<ContainerMetadata> getContainerComponentList(const uuids::uuid& componentUUID);
  unsigned long saveContainerInformations(const SPtr<ContainerMetadata> & containerInfos);
  void addContainerRef(const uuids::uuid& containerUUID);
  void releaseContainerRef(const uuids::uuid& containerUUID);

  unsigned long addComponentMetadata(SPtr<ComponentMetadata> aClass);
  int getNbComponentMetadatas() const;
  SPtr<ComponentMetadata> getComponentMetadata(int) const;
  SPtr<ComponentMetadata> findComponentMetadata(const uuids::uuid &) const;

  unsigned long addInterfaceMetadata(SPtr<InterfaceMetadata>);
  int getNbInterfaceMetadatas() const;
  SPtr<InterfaceMetadata> getInterfaceMetadata(int) const;
  SPtr<InterfaceMetadata> findInterfaceMetadata(const uuids::uuid&) const;

  static constexpr const char * UUID = "F905BCCD-9658-4871-87B8-B328C27675E0";


private:
  ComponentManager();
    ~ComponentManager()= default;
    ComponentManager(const ComponentManager&)= delete;
    ComponentManager& operator=(const ComponentManager&)= delete;
  static std::atomic<ComponentManager*> m_instance;
  static std::mutex m_mutex;

  unsigned long loadLibrary(boost::filesystem::path aPath);
  unsigned long getContainerComponentList(const char* containerFilePath ,SPtr<ContainerMetadata> containerInfos);

  //boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;

  std::vector<SPtr<ComponentMetadata>> m_componentsVector;
  std::vector<SPtr<InterfaceMetadata>> m_interfacesVector;
  std::map<uuids::uuid,SPtr<ComponentMetadata>> m_componentsMap;
  std::map<uuids::uuid,SPtr<InterfaceMetadata>> m_interfacesMap;
  std::map<uuids::uuid,SPtr<ContainerMetadata>> m_containerMap;
  std::map<uuids::uuid,boost::function<long(const uuids::uuid &, SRef<IComponentIntrospect>&)>> m_funcMap;
  bool m_libraryLoaded;
};

}}} //namespace org::bcom::xpcf

#endif
