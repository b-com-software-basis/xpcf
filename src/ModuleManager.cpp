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
 * @date 2017-11-22
 */

#include "xpcf/core/Exception.h"
#include "ModuleManager.h"
#include "xpcf/module/ModuleFactory.h"
#include "PathBuilder.h"
#include <iostream>
#include <fstream>
#include <boost/dll.hpp>
#include <boost/locale.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/function.hpp>
#include "tinyxmlhelper.h"

using namespace std;

namespace org { namespace bcom { namespace xpcf {

std::atomic<ModuleManager*> ModuleManager::m_instance;
std::mutex ModuleManager::m_mutex;

ModuleManager * ModuleManager::instance()
{

    ModuleManager* moduleMgrInstance= m_instance.load(std::memory_order_acquire);
    if ( !moduleMgrInstance ){
        std::lock_guard<std::mutex> myLock(m_mutex);
        moduleMgrInstance = m_instance.load(std::memory_order_relaxed);
        if ( !moduleMgrInstance ){
            moduleMgrInstance= new ModuleManager();
            m_instance.store(moduleMgrInstance, std::memory_order_release);
        }
    }
    return moduleMgrInstance;
}

SRef<IModuleManager> getModuleManagerInstance()
{
    SRef<xpcf::IComponentIntrospect> lrIComponentIntrospect = xpcf::ComponentFactory::create<ModuleManager>();
    return lrIComponentIntrospect->bindTo<IModuleManager>();
}

template<> ModuleManager* ComponentFactory::createInstance()
{
    return ModuleManager::instance();
}

ModuleManager::ModuleManager():ComponentBase(toUUID<ModuleManager>())
{
    addInterface<IModuleManager>(this);
}

void ModuleManager::unloadComponent ()
{
}

SPtr<ModuleMetadata> ModuleManager::introspectModule(const char* moduleName, const char* moduleFilePath)
{
    SPtr<ModuleMetadata> moduleInfos;
    fs::path filePath = PathBuilder::buildModuleFilePath(moduleName, moduleFilePath);
    try {
        boost::function<const char* (void)> getModuleUUID = boost::dll::import<const char * (void)>(
                    filePath, XPCF_GETMODULEUUID, boost::dll::load_mode::append_decorations | boost::dll::load_mode::load_with_altered_search_path
                    );
        boost::function<const char* (void)> getModuleName = boost::dll::import<const char * (void)>(
                    filePath, XPCF_GETMODULENAME, boost::dll::load_mode::append_decorations| boost::dll::load_mode::load_with_altered_search_path
                    );
        boost::function<const char* (void)> getModuleDescription = boost::dll::import<const char * (void)>(
                    filePath, XPCF_GETMODULEDESCRIPTION, boost::dll::load_mode::append_decorations| boost::dll::load_mode::load_with_altered_search_path
                    );
        boost::function<void (SRef<IModuleIndex>)> getModuleIndex = boost::dll::import<void (SRef<IModuleIndex>)>(
                    filePath, XPCF_GETMODULEINDEX, boost::dll::load_mode::append_decorations| boost::dll::load_mode::load_with_altered_search_path
                    );
        if (getModuleUUID && getModuleName && getModuleIndex) {
            std::string moduleUUIDStr = getModuleUUID();
            std::string moduleName = getModuleName();
            std::string moduleDescription = getModuleDescription();
            xpcf::uuids::uuid moduleUUID = toUUID(moduleUUIDStr.c_str());
            if (m_moduleMap.find(moduleUUID) == m_moduleMap.end()) {
                moduleInfos = utils::make_shared<ModuleMetadata>(moduleName.c_str(), moduleUUIDStr.c_str(), moduleDescription.c_str(), moduleFilePath);
                getModuleIndex(moduleInfos);
                m_moduleMap[moduleUUID] = moduleInfos;
            }
            else { //Module has already been loaded in the library
                moduleInfos = m_moduleMap.at(moduleUUID);
            }
        }
    }
    catch (boost::system::system_error & e) {
        throw ModuleException(e.what());
    }
    return moduleInfos;
}

void ModuleManager::addModuleRef(const uuids::uuid& moduleUUID)
{

}

void ModuleManager::releaseModuleRef(const uuids::uuid& moduleUUID)
{

}

SRef<IComponentIntrospect> ModuleManager::createComponent(SPtr<ModuleMetadata> moduleInfos, const uuids::uuid& componentUUID)
{
    SRef<IComponentIntrospect> componentRef;
    boost::dll::shared_library shlib;
    try {
        shlib.load(moduleInfos->getFullPath(), boost::dll::load_mode::append_decorations | boost::dll::load_mode::load_with_altered_search_path);
    }
    catch (boost::system::system_error & e) {
        throw ModuleException(e.what());
    }
    if (shlib.has(XPCF_GETCOMPONENT)) {
        if (m_funcMap.find(moduleInfos->getUUID()) == m_funcMap.end()) {
            m_funcMap[moduleInfos->getUUID()]=boost::dll::import<XPCFErrorCode(const uuids::uuid &, SRef<IComponentIntrospect>&)>(
                        moduleInfos->getFullPath(), XPCF_GETCOMPONENT, boost::dll::load_mode::append_decorations | boost::dll::load_mode::load_with_altered_search_path
                        );
        }

        if (m_funcMap.at(moduleInfos->getUUID())) {
            XPCFErrorCode errCode = m_funcMap.at(moduleInfos->getUUID())(componentUUID, componentRef);
            if (componentRef) {
                addModuleRef(moduleInfos->getUUID());
            }
        }

    }
    else {
        throw ModuleException("No XPCF_getComponent entry point declared for module " + moduleInfos->getFullPath().string() , XPCFErrorCode::_ERROR_MODULE_NOGETCOMPONENT);
    }
    return componentRef;
}

std::vector<SRef<InterfaceMetadata>> ModuleManager::getComponentInterfaceList(SPtr<ModuleMetadata> moduleInfos, const uuids::uuid& componentUUID)
{
    std::vector<SRef<InterfaceMetadata>> interfaceVector;
    SRef<xpcf::IComponentIntrospect> rIntrospect = createComponent(moduleInfos, componentUUID);

    for (uuids::uuid interfaceUUID : rIntrospect->getInterfaces()) {
        SRef<InterfaceMetadata> interfaceMdata = utils::make_shared<InterfaceMetadata>(rIntrospect->getMetadata(interfaceUUID));
        interfaceVector.push_back(interfaceMdata);
    }
    return interfaceVector;
}

// TODO : study which method signature ? with module ID ? or ModuleMetadata ?
// What about default configuration for the component ?
XPCFErrorCode ModuleManager::saveModuleInformations(const char * xmlFilePath, const SPtr<ModuleMetadata> & moduleInfos)
{
    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;
    tinyxml2::XMLDocument xmlDoc;
    // root node creation will occur only for non existing files, otherwise the root node will be loaded and elements for the module will be inserted in the xml tree
    tinyxml2::XMLNode * xmlRoot;
    enum tinyxml2::XMLError loadOkay = xmlDoc.LoadFile(xmlFilePath);
    if (loadOkay == 0) {
        xmlRoot = xmlDoc.RootElement();
    }
    else {
        std::cout<<"ModuleManager::saveModuleInformations ===> failed loading doc "<<xmlFilePath<<std::endl;
        xmlRoot = xmlDoc.NewElement("xpcf-registry");
        xmlDoc.InsertFirstChild(xmlRoot);
    }
    try {
        //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Parsing XML from "<<modulePath<<" config file";
        //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"NOTE : Command line arguments are overloaded with config file parameters";
        //TODO : check each element exists before using it !
        // TODO : before adding a module, we MUST check if a module node with the same UUID exists !!
        std::cout<<"ModuleManager::saveModuleInformations ===> creating module XmlNode"<<std::endl;
        tinyxml2::XMLElement * xmlModuleElt = xmlDoc.NewElement("module");
        xmlModuleElt->SetAttribute("uuid", boost::uuids::to_string(moduleInfos->getUUID()).c_str());
        xmlModuleElt->SetAttribute("name", moduleInfos->name());
        xmlModuleElt->SetAttribute("path", moduleInfos->getPath());
        xmlModuleElt->SetAttribute("description", moduleInfos->description());
        for (SRef<ComponentMetadata> componentInfo : moduleInfos->getComponentsMetadata()) {
            std::cout<<"ModuleManager::saveModuleInformations ===> retrieving component #"<<componentInfo->getUUID()<<std::endl;
            std::cout<<"ModuleManager::saveModuleInformations ===> creating component XmlNode"<<std::endl;
            tinyxml2::XMLElement * xmlComponentElt = xmlDoc.NewElement("component");
            xmlComponentElt->SetAttribute("uuid", boost::uuids::to_string(componentInfo->getUUID()).c_str());
            xmlComponentElt->SetAttribute("name", componentInfo->name());
            xmlComponentElt->SetAttribute("description", componentInfo->description());
            xmlModuleElt->InsertEndChild(xmlComponentElt);
            std::vector<SRef<InterfaceMetadata>> interfaceList = getComponentInterfaceList(moduleInfos, componentInfo->getUUID());
            for (SRef<InterfaceMetadata> interface : interfaceList) {
                std::cout<<"ModuleManager::saveModuleInformations ===> creating interface XmlNode"<<std::endl;
                tinyxml2::XMLElement * xmlInterfaceElt = xmlDoc.NewElement("interface");
                xmlInterfaceElt->SetAttribute("uuid", boost::uuids::to_string(interface->getUUID()).c_str());
                xmlInterfaceElt->SetAttribute("name", interface->name());
                xmlInterfaceElt->SetAttribute("description", interface->description());
                xmlComponentElt->InsertEndChild(xmlInterfaceElt);
            }
        }
        xmlRoot->InsertEndChild(xmlModuleElt);
        tinyxml2::XMLError eResult = xmlDoc.SaveFile(xmlFilePath);
    }
    catch (const std::runtime_error & e) {
        //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"XML parsing file "<<modulePath<<" failed with error : "<<e.what();
        return XPCFErrorCode::_FAIL;
    }
    return result;
}


}}}
