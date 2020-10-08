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

#include "xpcf/api/IConfigurable.h"
#include "ModuleManager.h"
#include "ComponentManager.h"
#include "xpcf/component/ComponentFactory.h"
#include "xpcf/core/Exception.h"
#include "PathBuilder.h"
#include "PropertyManager.h"

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
    m_factory =  ComponentFactory::create<Factory>()->bindTo<IFactory>();
    m_registry = m_factory->resolve<IRegistry>();
    m_aliasManager = m_factory->resolve<IAliasManager>();
    m_propertyManager =  m_factory->resolve<IPropertyManager>();
#ifdef XPCF_WITH_LOGS
    m_logger.add_attribute("ClassName", boost::log::attributes::constant<std::string>("ComponentManager"));
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Constructor ComponentManager::ComponentManager () called!";
#endif
}

uuids::uuid ComponentManager::getModuleUUID(const uuids::uuid & componentUUID) const
{
    return m_registry->getModuleUUID(componentUUID);
}

void ComponentManager::releaseComponent(uuids::uuid componentUUID)
{
    //Note : release the module ref, or count moduleref per component uuid to check ??
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
    m_registry->clear();
    m_factory->clear();
    m_aliasManager->clear();
    m_libraryLoaded = false;
}

XPCFErrorCode ComponentManager::load()
{
    const char * registryPath = getenv( "XPCF_REGISTRY_PATH" );
    if (registryPath != nullptr) {
        load(registryPath);
    }
    return load<fs::recursive_directory_iterator>(PathBuilder::getXPCFHomePath());
}

XPCFErrorCode ComponentManager::load(const char* libraryFilePath)
{
    if (libraryFilePath == nullptr) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }

    fs::path libraryConfigFilePath = PathBuilder::replaceRootEnvVars(libraryFilePath);
    if (libraryConfigFilePath.is_relative()) {
        // relative file should exists from the current process working dir
        if (!fs::exists(libraryConfigFilePath)) {
            // relative file is not found from the cwd : search in xpcf home dir
            libraryConfigFilePath = PathBuilder::getXPCFHomePath() / libraryConfigFilePath;
        }
    }
    return loadLibrary(libraryConfigFilePath);
}

template <class T> XPCFErrorCode ComponentManager::load(fs::path folderPath)
{
    //TODO : what strategy to report error of load for a dedicated file but load others ?
    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;
    static_assert(std::is_same<T,fs::directory_iterator>::value || std::is_same<T,fs::recursive_directory_iterator>::value,
                  "Type passed to ComponentManager::load is neither a directory_iterator nor a recursive_directory_iterator");
    for (fs::directory_entry& x : T(folderPath)) {
        if (is_regular_file(x.path())) {
            if (x.path().extension() == ".xml") {
                loadLibrary(x.path());
            }
        }
    }
    return result;
}

XPCFErrorCode ComponentManager::load(const char* folderPathStr, bool bRecurse)
{
    if (folderPathStr == nullptr) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }

    fs::path folderPath = PathBuilder::replaceRootEnvVars(folderPathStr);

    if ( ! fs::exists(folderPath)) {
        return XPCFErrorCode::_FAIL;
    }

    if (fs::is_directory(folderPath)) {
        if (bRecurse) {
            load<fs::recursive_directory_iterator>(folderPath);
        }
        else {
            load<fs::directory_iterator>(folderPath);
        }
    }
    else {
        loadLibrary(folderPath);
    }

    if (!m_libraryLoaded) {
        return XPCFErrorCode::_FAIL;
    }
    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode ComponentManager::loadModuleMetadata(const char * moduleName,
                                                   const char * moduleFilePath)
{
    SRef<ModuleMetadata> moduleInfos = getModuleManagerInstance()->introspectModule(moduleName, moduleFilePath);
    m_registry->declareModuleMetadata(moduleInfos);
    return XPCFErrorCode::_SUCCESS;
}

template <class T> XPCFErrorCode ComponentManager::loadModules(fs::path folderPath)
{
    //TODO : what strategy to report error of load for a dedicated file but load others ?
    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;
    static_assert(std::is_same<T,fs::directory_iterator>::value || std::is_same<T,fs::recursive_directory_iterator>::value,
                  "Type passed to ComponentManager::load is neither a directory_iterator nor a recursive_directory_iterator");
    for (fs::directory_entry& x : T(folderPath)) {
        if (PathBuilder::is_shared_library(x.path())) {
                fs::path modulePath = x.path().parent_path();
                fs::path moduleName = x.path().filename();
                fs::detail::utf8_codecvt_facet utf8;
                result = loadModuleMetadata(moduleName.string(utf8).c_str(),modulePath.string(utf8).c_str());
        }
    }
    return result;
}

XPCFErrorCode ComponentManager::loadModules(const char * folderPathStr, bool bRecurse)
{
    if (folderPathStr == nullptr) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }

    fs::path folderPath = PathBuilder::buildModuleFolderPath(folderPathStr);

    if ( ! fs::exists(folderPath)) {
        return XPCFErrorCode::_FAIL;
    }

    if ( !fs::is_directory(folderPath)) {
        return XPCFErrorCode::_FAIL;
    }
    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;

    if (bRecurse) {
        result = loadModules<fs::recursive_directory_iterator>(folderPath);
    }
    else {
        result = loadModules<fs::directory_iterator>(folderPath);
    }
    return result;
}

SRef<IComponentIntrospect> ComponentManager::create(const uuids::uuid & componentUUID)
{
    SPtr<ModuleMetadata> moduleInfos = findModuleMetadata(getModuleUUID(componentUUID));
    SRef<IComponentIntrospect> componentRef = getModuleManagerInstance()->createComponent(moduleInfos, componentUUID);
    return componentRef;
}

void ComponentManager::inject(SRef<IInjectable> component)
{
    for (auto injectable : component->getInjectables()) {
        if (!injectable->optional() && !m_factory->bindExists(injectable)) {
            throw InjectableNotFoundException(injectable);
        }
        if (m_factory->bindExists(injectable)) {
            if (!injectable->isMulti()) {
                SRef<IComponentIntrospect> injectableRef = m_factory->resolve(injectable);
                injectable->inject(injectableRef);
            }
            else {
                SRef<IEnumerable<SRef<IComponentIntrospect>>> injectableRef = m_factory->resolveAll(injectable);
                injectable->inject(injectableRef);
            }
        }
    }
    component->onInjected();
}

SRef<IComponentIntrospect> ComponentManager::createComponent(const uuids::uuid & componentUUID)
{
    SRef<IComponentIntrospect> componentRef = create(componentUUID);
    inject(componentRef->bindTo<IInjectable>());

    fs::path configFilePath = m_propertyManager->getConfigPath(componentUUID);
    if (componentRef->implements<IConfigurable>() && ! configFilePath.empty()) {
        SRef<IConfigurable> iconf = componentRef->bindTo<IConfigurable>();
        iconf->configure(configFilePath.string().c_str());
    }
    return componentRef;
}


SRef<IComponentIntrospect> ComponentManager::createComponent(const char * instanceName, const uuids::uuid & componentUUID)
{
    SRef<IComponentIntrospect> componentRef = create(componentUUID);
    inject(componentRef->bindTo<IInjectable>());

    fs::path configFilePath = m_propertyManager->getConfigPath(componentUUID);
    if (componentRef->implements<IConfigurable>() && ! configFilePath.empty()) {
        SRef<IConfigurable> iconf = componentRef->bindTo<IConfigurable>();
        iconf->configure(configFilePath.string().c_str(), instanceName);
    }
    return componentRef;
}

SRef<IComponentIntrospect> ComponentManager::resolve(const uuids::uuid & interfaceUUID)
{
    if (!m_factory->bindExists(interfaceUUID)) {
        throw InjectableNotFoundException("Unable to resolve component from interface UUID = " + uuids::to_string(interfaceUUID));
    }
    SRef<IComponentIntrospect> componentRef = m_factory->resolve(interfaceUUID);
    return componentRef;
}


SRef<IComponentIntrospect> ComponentManager::resolve(const uuids::uuid & interfaceUUID, const char * name)
{
    if (!m_factory->bindExists(interfaceUUID)) {
        throw InjectableNotFoundException("Unable to resolve component from interface UUID = " + uuids::to_string(interfaceUUID));
    }
    SRef<IComponentIntrospect> componentRef = m_factory->resolve(interfaceUUID, name);
    return componentRef;
}

void ComponentManager::bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID, IComponentManager::Scope scope)
{
    m_factory->bind(interfaceUUID,instanceUUID,scope);
}

void ComponentManager::bind(const char * name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID, IComponentManager::Scope scope)
{
    m_factory->bind(name, interfaceUUID, instanceUUID, scope);
}

void ComponentManager::bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                  const uuids::uuid & instanceUUID, IComponentManager::Scope scope)

{
    m_factory->bind(targetComponentUUID, interfaceUUID, instanceUUID, scope);
}

void ComponentManager::bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
                  const uuids::uuid & instanceUUID, IComponentManager::Scope scope)
{
    m_factory->bind(targetComponentUUID, name, interfaceUUID, instanceUUID, scope);
}

void ComponentManager::bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                    const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                    IComponentManager::Scope scope)
{
    m_factory->bind(interfaceUUID, instanceUUID, factoryFunc, scope);
}

void ComponentManager::bind(const char * name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                    const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                    IComponentManager::Scope scope)
{
    m_factory->bind(name, interfaceUUID, instanceUUID, factoryFunc, scope);
}

void ComponentManager::bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
          const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
          const uuids::uuid & instanceUUID, IComponentManager::Scope scope)
{
    m_factory->bind(targetComponentUUID, interfaceUUID, factoryFunc, instanceUUID, scope);
}

void ComponentManager::bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
          const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
          const uuids::uuid & instanceUUID, IComponentManager::Scope scope)
{
    m_factory->bind(targetComponentUUID, name, interfaceUUID, factoryFunc, instanceUUID, scope);
}

template <class T>
SPtr<T> findMetadata(const uuids::uuid & elementUUID, const std::map<uuids::uuid, SPtr<T>> & elementMap)
{
    SPtr<T> result;
    if (elementMap.find(elementUUID) != elementMap.end()) {
        result = elementMap.at(elementUUID);
    }
    return result;
}

// Given a UUID, we return a ComponentMetadata
// if no component is found with the UUID <clsid>
// a NULL smart pointer is returned.
//

SPtr<ModuleMetadata> ComponentManager::findModuleMetadata(const uuids::uuid & moduleUUID) const
{
    return m_registry->findModuleMetadata(moduleUUID);
}

SPtr<ComponentMetadata> ComponentManager::findComponentMetadata(const uuids::uuid & componentUUID) const
{
    return m_registry->findComponentMetadata(componentUUID);
}

SPtr<InterfaceMetadata> ComponentManager::findInterfaceMetadata(const uuids::uuid& interfaceUUID) const
{
    return m_registry->findInterfaceMetadata(interfaceUUID);
}

// Load the library from the file given
// by <modulePath>
//
/********* The library file looks like this*************************************************
<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<xpcf-registry>
    <module uuid="E8CFAED2-1D86-4A5E-BDC9-389CFB9C38FA" name="xpcftest01" path="folderpath/">
        <component uuid="097fee80-7bd4-47c2-9cc7-c1750e7beb2a" description="C0 component">
                TOADD: <constructor arg-0=UUIDOTHERCOMPONENT , arg-1=configoptions/>
                <interface uuid="87fb1573-defe-4c8f-8cba-304b888639cc" description="IComponentIntrospect"/>
                <interface uuid="c3eb5521-16ee-458e-9e77-157827753e46" description="I0 openCV scale interface"/>
                <interface uuid="e0b0a9af-a941-408e-aa10-cac5055af75a" description="I1 openCV rotate interface"/>
            </component>
    </module>
    <module uuid="7c8f9f3a-33f6-11d5-bc7f-00d0b7a62da9" name="xpcftest02" path="folderpath2/">
        <component uuid="cf0f569a-2f66-11d5-bc7f-00d0b7a62da9" description="xxx">
                <interface uuid="5f7c9b5e-ee9f-11d3-8010-000629ee2d57" description="IComponentIntrospect"/>
                <interface uuid="9db144b8-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
                <interface uuid="c5d2b99a-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
        </component>
    </module>
    <module uuid="b89994bc-3424-11d5-bc7f-00d0b7a62da9" name="xpcftest03" path="folderpath3/">
        <component uuid="cf0f569a-2f66-11d5-bc7f-00d0b7a62da9" description="xxx">
                <interface uuid="5f7c9b5e-ee9f-11d3-8010-000629ee2d57" description="IComponentIntrospect"/>
                <interface uuid="9db144b8-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
                <interface uuid="c5d2b99a-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
        </component>
        <component uuid="c2095c04-2f69-11d5-bc7f-00d0b7a62da9" description="xxx">
                <interface uuid="5f7c9b5e-ee9f-11d3-8010-000629ee2d57" description="IComponentIntrospect"/>
                <interface uuid="c5d2b99a-2f67-11d5-bc7f-00d0b7a62da9" description="xxx"/>
        </component>
    </module>
</xpcf-library>

*********************************************************************************************/
XPCFErrorCode ComponentManager::loadLibrary(fs::path configurationFilePath)
{
    if ( ! fs::exists(configurationFilePath)) {
        return XPCFErrorCode::_FAIL;
    }
    if ( configurationFilePath.extension() != ".xml") {
        return XPCFErrorCode::_FAIL;
    }

    XPCFErrorCode result = XPCFErrorCode::_FAIL;
    tinyxml2::XMLDocument doc;
    enum tinyxml2::XMLError loadOkay = doc.LoadFile(configurationFilePath.string().c_str());
    if (loadOkay == 0) {
        try {
#ifdef XPCF_WITH_LOGS
            BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Parsing XML from "<<configurationFilePath<<" config file";
            BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"NOTE : Command line arguments are overloaded with config file parameters";
#endif
            //TODO : check each element exists before using it !
            // a check should be performed upon announced module uuid and inner module uuid
            // check xml node is xpcf-registry first !
            tinyxml2::XMLElement * rootElt = doc.RootElement();
            string rootName = rootElt->Value();
            if (rootName != "xpcf-registry" && rootName != "xpcf-configuration") {
                return XPCFErrorCode::_ERROR_RANGE;
            }
            result = XPCFErrorCode::_SUCCESS;
            const char * autoAliasProp = rootElt->Attribute("autoAlias");
            m_registry->enableAutoAlias(false);
            if (autoAliasProp != nullptr) {
                const string autoAliasStr(autoAliasProp);
                if (autoAliasStr == "true") {
                    m_registry->enableAutoAlias(true);
                }
            }
            processXmlNode(rootElt, "module", std::bind(&IRegistry::declareModule, m_registry.get(), _1));
            processXmlNode(rootElt, "aliases", std::bind(&IAliasManager::declareAliases, m_aliasManager.get(), _1));
            processXmlNode(rootElt, "factory", std::bind(&IFactory::declareFactory, m_factory.get(), _1));
            std::function<void(tinyxml2::XMLElement*,  const fs::path &)> declareConfigureFunc = std::bind(&IPropertyManager::declareConfiguration,  m_propertyManager.get(), _1,_2);
            processXmlNode<const fs::path &>(rootElt, "configuration", declareConfigureFunc, configurationFilePath);
            std::function<void(tinyxml2::XMLElement*,  const fs::path &)> declarePropertiesFunc = std::bind(&IPropertyManager::declareProperties,  m_propertyManager.get(), _1,_2);
            processXmlNode<const fs::path &>(rootElt, "properties", declarePropertiesFunc, configurationFilePath);
        }
        catch (const xpcf::Exception & e) {
            return e.getErrorCode();
        }
        catch (const std::runtime_error & e) {
#ifdef XPCF_WITH_LOGS
            BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"XML parsing file "<<configurationFilePath<<" failed with error : "<<e.what();
#endif
            return XPCFErrorCode::_FAIL;
        }
    }
    m_libraryLoaded = (result == XPCFErrorCode::_SUCCESS);
    return result;
}

const IEnumerable<SPtr<ModuleMetadata>> & ComponentManager::getModulesMetadata() const
{
    return m_registry->getModulesMetadata();
}

const IEnumerable<SPtr<InterfaceMetadata>> & ComponentManager::getInterfacesMetadata() const
{
    return m_registry->getInterfacesMetadata();
}

}}} //namespace org::bcom::xpcf
