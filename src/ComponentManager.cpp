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

#include <xpcf/api/IConfigurable.h>
#include "private/xpcf/ModuleManager.h"
#include "private/xpcf/ComponentManager.h"
#include <xpcf/component/ComponentFactory.h>
#include <xpcf/core/Exception.h>
#include "private/xpcf/PathBuilder.h"
#include "private/xpcf/PropertyManager.h"

#include <iostream>
#include <fstream>
#include <functional>
using namespace std;
using std::placeholders::_1;
using std::placeholders::_2;
//namespace logging = boost::log;

#include <boost/filesystem.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
namespace fs = boost::filesystem;
#ifdef XPCF_WITH_LOGS
namespace logging = boost::log;
#endif

namespace org { namespace bcom { namespace xpcf {

std::atomic<ComponentManager*> ComponentManager::m_instance;
std::mutex ComponentManager::m_mutex;

ComponentManager * ComponentManager::instance()
{
    ComponentManager* compMgrInstance= m_instance.load(std::memory_order_acquire);
    if ( !compMgrInstance ){
        std::lock_guard<std::mutex> myLock(m_mutex);
        compMgrInstance = m_instance.load(std::memory_order_relaxed);
        if ( !compMgrInstance ){
            compMgrInstance= new ComponentManager();
            m_instance.store(compMgrInstance, std::memory_order_release);
        }
    }
    return compMgrInstance;
}

SRef<IComponentManager> getComponentManagerInstance()
{
    SRef<xpcf::IComponentIntrospect> lrIComponentIntrospect = xpcf::ComponentFactory::create<ComponentManager>();
    return lrIComponentIntrospect->bindTo<IComponentManager>();
}

SRef<IComponentIntrospect> createInstance(const uuids::uuid& componentUUID)
{

    SRef<xpcf::IComponentManager> xpcfComponentManager = xpcf::getComponentManagerInstance();
    SRef<IComponentIntrospect> component = xpcfComponentManager->createComponent(componentUUID);
    return component;
}

SRef<IComponentIntrospect> createInstance(const char * instanceName, const uuids::uuid& componentUUID)
{
    SRef<xpcf::IComponentManager> xpcfComponentManager = xpcf::getComponentManagerInstance();
    SRef<IComponentIntrospect> component = xpcfComponentManager->createComponent(instanceName, componentUUID);
    return component;
}

template<> ComponentManager* ComponentFactory::createInstance()
{
    return ComponentManager::instance();
}

ComponentManager::ComponentManager():ComponentBase(toUUID<ComponentManager>()),m_libraryLoaded(false)
{
    declareInterface<IComponentManager>(this);
    m_factory =  ComponentFactory::create<Factory>()->bindTo<AbstractFactory>();
    m_propertyManager =  m_factory->resolve<IPropertyManager>();
#ifdef XPCF_WITH_LOGS
    m_logger.add_attribute("ClassName", boost::log::attributes::constant<std::string>("ComponentManager"));
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Constructor ComponentManager::ComponentManager () called!";
#endif
}

uuids::uuid ComponentManager::getModuleUUID(const uuids::uuid & componentUUID) const
{
    return m_factory->bindTo<IRegistryManager>()->getModuleUUID(componentUUID);
}

void ComponentManager::releaseComponent(uuids::uuid componentUUID)
{
    //Note : release the module ref, or count moduleref per component uuid to check ??
    //Caution : no release must occur for bindLocal'd components
    ModuleManager::instance()->releaseModuleRef(getModuleUUID(componentUUID));
}

void ComponentManager::unloadComponent ()
{
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"ComponentManager::unload () called!";
#endif
}

void ComponentManager::clear()
{
    m_propertyManager->clear();
    m_factory->clear();
    m_libraryLoaded = false;
}

SRef<IFactory> ComponentManager::getFactory()
{
    return m_factory;
}

XPCFErrorCode ComponentManager::load()
{
    return m_factory->load();
}

XPCFErrorCode ComponentManager::load(const char* libraryFilePath)
{
    return m_factory->load(libraryFilePath);
}

XPCFErrorCode ComponentManager::load(const char* folderPathStr, bool bRecurse)
{
    return m_factory->load(folderPathStr, bRecurse);
}

SRef<IComponentIntrospect> ComponentManager::createComponent(const uuids::uuid & componentUUID)
{
    return m_factory->createComponent(componentUUID);
}

SRef<IComponentIntrospect> ComponentManager::createComponent(const char * instanceName, const uuids::uuid & componentUUID)
{
    return m_factory->createComponent(instanceName, componentUUID);
}

// Given a UUID, we return a ComponentMetadata
// if no component is found with the UUID <clsid>
// a NULL smart pointer is returned.
//

SPtr<ModuleMetadata> ComponentManager::findModuleMetadata(const uuids::uuid & moduleUUID) const
{
    return m_factory->bindTo<IRegistryManager>()->findModuleMetadata(moduleUUID);
}

SPtr<ComponentMetadata> ComponentManager::findComponentMetadata(const uuids::uuid & componentUUID) const
{
    return m_factory->bindTo<IRegistryManager>()->findComponentMetadata(componentUUID);
}

SPtr<InterfaceMetadata> ComponentManager::findInterfaceMetadata(const uuids::uuid& interfaceUUID) const
{
    return m_factory->bindTo<IRegistryManager>()->findInterfaceMetadata(interfaceUUID);
}

const IEnumerable<SPtr<ModuleMetadata>> & ComponentManager::getModulesMetadata() const
{
    return m_factory->bindTo<IRegistryManager>()->getModulesMetadata();
}

const IEnumerable<SPtr<InterfaceMetadata>> & ComponentManager::getInterfacesMetadata() const
{
    return m_factory->bindTo<IRegistryManager>()->getInterfacesMetadata();
}

}}} //namespace org::bcom::xpcf
