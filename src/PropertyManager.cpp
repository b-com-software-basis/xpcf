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
#include "xpcf/api/IModuleManager.h"
#include "xpcf/core/Exception.h"
#include "xpcf/module/ModuleFactory.h"
#include "PropertyManager.h"
#include "PathBuilder.h"

#include "tinyxmlhelper.h"
#include <iostream>
#include <fstream>

using namespace std;
using placeholders::_1;
using placeholders::_2;
//namespace logging = boost::log;

namespace fs = boost::filesystem;
#ifdef XPCF_WITH_LOGS
namespace logging = boost::log;
#endif

namespace org { namespace bcom { namespace xpcf {

std::atomic<PropertyManager*> PropertyManager::m_instance;
std::mutex PropertyManager::m_mutex;

PropertyManager * PropertyManager::instance()
{

    PropertyManager* compMgrInstance= m_instance.load(std::memory_order_acquire);
    if ( !compMgrInstance ){
        std::lock_guard<std::mutex> myLock(m_mutex);
        compMgrInstance = m_instance.load(std::memory_order_relaxed);
        if ( !compMgrInstance ){
            compMgrInstance= new PropertyManager();
            m_instance.store(compMgrInstance, std::memory_order_release);
        }
    }
    return compMgrInstance;
}


SRef<IPropertyManager> getPropertyManagerInstance()
{
    SRef<xpcf::IComponentIntrospect> lrIComponentIntrospect = xpcf::ComponentFactory::create<PropertyManager>();
    return lrIComponentIntrospect->bindTo<IPropertyManager>();
}

template<> PropertyManager* ComponentFactory::createInstance()
{
    return PropertyManager::instance();
}

constexpr const char * XMLCOMPONENTNODE = "component";
constexpr const char * XMLCONFIGURENODE = "configure";

PropertyManager::PropertyManager():ComponentBase(toUUID<PropertyManager>())
{
    declareInterface<IPropertyManager>(this);
    declareInjectable<IAliasManager>(m_aliasManager);
    declareInjectable<IRegistry>(m_registry);
#ifdef XPCF_WITH_LOGS
    m_logger.add_attribute("ClassName", boost::log::attributes::constant<std::string>("PropertyManager"));
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Constructor PropertyManager::PropertyManager () called!";
#endif
}

void PropertyManager::unloadComponent ()
{
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"PropertyManager::unload () called!";
#endif
}


map<IProperty::PropertyType, std::string> propertyTypeToStrMap = {
    {IProperty::IProperty_NONE,""},
    {IProperty::IProperty_INTEGER,"int"},
    {IProperty::IProperty_UINTEGER,"uint"},
    {IProperty::IProperty_LONG,"long"},
    {IProperty::IProperty_ULONG,"ulong"},
    {IProperty::IProperty_CHARSTR,"string"},
    {IProperty::IProperty_UNICODESTR,"wstring"},
    {IProperty::IProperty_FLOAT,"float"},
    {IProperty::IProperty_DOUBLE,"double"},
    {IProperty::IProperty_STRUCTURE,"structure"}
};

map<IProperty::AccessSpecifier, std::string> propertyAccessToStrMap = {
    {IProperty::IProperty_IN,"w"},
    {IProperty::IProperty_OUT,"r"},
    {IProperty::IProperty_INOUT,"rw"}
};

void PropertyManager::clear()
{
    m_moduleConfigMap.clear();
    m_componentConfigMap.clear();
}

void PropertyManager::declareComponent(tinyxml2::XMLElement * xmlElt, const fs::path & configFilePath)
{
    string componentUuidStr = xmlElt->Attribute("uuid");
    uuids::uuid componentUuid = toUUID(componentUuidStr);
    uuids::uuid moduleUuid = m_registry->getModuleUUID(componentUuid);
    if (m_moduleConfigMap.find(moduleUuid) == m_moduleConfigMap.end()) {
        m_moduleConfigMap[moduleUuid] = configFilePath;
    }
    else {
        fs::path & moduleConfigurationPath = m_moduleConfigMap[moduleUuid];
        if (moduleConfigurationPath != configFilePath) {
            m_componentConfigMap[componentUuid] = configFilePath;
        }
    }
}

void PropertyManager::declareConfigure(tinyxml2::XMLElement * xmlElt, const fs::path & configFilePath)
{
    string componentAttrValue = xmlElt->Attribute("component");
    uuids::uuid componentUuid;
    if (m_aliasManager->aliasExists(IAliasManager::Type::Component, componentAttrValue)) {
        componentUuid = m_aliasManager->resolveComponentAlias(componentAttrValue);
    }
    else {
        componentUuid =  toUUID(componentAttrValue);
    }
    uuids::uuid moduleUuid = m_registry->getModuleUUID(componentUuid);
    if (m_moduleConfigMap.find(moduleUuid) == m_moduleConfigMap.end()) {
        m_moduleConfigMap[moduleUuid] = configFilePath;
    }
    else {
        fs::path & moduleConfigurationPath = m_moduleConfigMap[moduleUuid];
        if (moduleConfigurationPath != configFilePath) {
            m_componentConfigMap[componentUuid] = configFilePath;
        }
    }
}

void PropertyManager::declareConfiguration(tinyxml2::XMLElement * xmlElt, const fs::path & configFilePath)
{
    std::function<void(tinyxml2::XMLElement*,  const fs::path &)> declareComponentFunc = std::bind(&PropertyManager::declareComponent, this, _1,_2);
    processXmlNode<const fs::path &>(xmlElt, XMLCOMPONENTNODE, declareComponentFunc, configFilePath);
}

void PropertyManager::declareProperties(tinyxml2::XMLElement * xmlElt, const fs::path & configFilePath)
{
    std::function<void(tinyxml2::XMLElement*,  const fs::path &)> declareConfigureFunc = std::bind(&PropertyManager::declareConfigure, this, _1,_2);
    processXmlNode<const fs::path &>(xmlElt, XMLCONFIGURENODE, declareConfigureFunc, configFilePath);
}

fs::path PropertyManager::getConfigPath(uuids::uuid componentUUID) const
{
    if (m_componentConfigMap.find(componentUUID) != m_componentConfigMap.end()) {
         return m_componentConfigMap.at(componentUUID);
    }
    try {
        uuids::uuid moduleUUID = m_registry->getModuleUUID(componentUUID);
        if (m_moduleConfigMap.find(moduleUUID) == m_moduleConfigMap.end()) {
            //log("No configuration path found for module "+uuids::to_string(moduleUUID));
            // return empty path
            return fs::path();
        }
        return m_moduleConfigMap.at(moduleUUID);
    }
    catch (...) { // no module associated with componentUUID : componentUUID can be declared from bindLocal
        //log("No module neither configuration path found for component "+uuids::to_string(componentUUID));
        // return empty path
        return fs::path();
    }
}

XPCFErrorCode configureValue(string valueStr, SRef<IProperty> property,uint32_t valueIndex)
{
    if ( valueStr.empty() ) {
        return XPCFErrorCode::_ERROR_INVALID_ARGUMENT;
    }

    try {
        switch (property->getType()) {
        case IProperty::IProperty_INTEGER:
                property->setIntegerValue(std::stol(valueStr,nullptr,0), valueIndex);
            break;
        case IProperty::IProperty_UINTEGER:
                property->setUnsignedIntegerValue(std::stoul(valueStr,nullptr,0), valueIndex);
            break;

        case IProperty::IProperty_LONG:
                property->setLongValue(std::stoll(valueStr,nullptr,0), valueIndex);
            break;
        case IProperty::IProperty_ULONG:
                property->setUnsignedLongValue(std::stoull(valueStr,nullptr,0), valueIndex);
            break;
        case IProperty::IProperty_FLOAT:
             property->setFloatingValue(std::stof(valueStr,nullptr), valueIndex);
            break;
        case IProperty::IProperty_DOUBLE:
            property->setDoubleValue(std::stod(valueStr,nullptr), valueIndex);
            break;
        case IProperty::IProperty_CHARSTR:
                property->setStringValue(valueStr.c_str(), valueIndex);
            break;
        case IProperty::IProperty_UNICODESTR:
            //??????
                //property->setUnicodeStringValue(getUnicodeStringValue(i), valueIndex);
            break;
        default:
            return XPCFErrorCode::_ERROR_TYPE;
            break;
        }
    }
    catch (const std::invalid_argument & e) {
        return XPCFErrorCode::_ERROR_TYPE;
    }
    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode configureElement(tinyxml2::XMLElement *element, SRef<IPropertyMap> nodeElement);

XPCFErrorCode configureProperty(tinyxml2::XMLElement *propertyElt, SRef<IPropertyMap> nodeElement)
{
    string propertyName = propertyElt->Attribute("name");
    if ( propertyName.empty() ) {
        return XPCFErrorCode::_ERROR_INVALID_ARGUMENT;
    }

    SRef<IProperty> currentProperty = nodeElement->at(propertyName.c_str());
    if (! currentProperty) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }

    string propertyType = propertyElt->Attribute("type");
    if ( propertyName.empty() ) {
        return XPCFErrorCode::_ERROR_INVALID_ARGUMENT;
    }

    if (propertyType == "structure") {
        if (currentProperty->getType() != IProperty::IProperty_STRUCTURE) {
            return XPCFErrorCode::_ERROR_TYPE;
        }

        SRef<IPropertyMap> propertyNode = currentProperty->getStructureValue();
        return configureElement(propertyElt,propertyNode);
    }

    const char * propertyCharStr = propertyElt->Attribute("value");
    if (propertyCharStr != nullptr) {
        string propertyValue = propertyCharStr;
        return configureValue(propertyValue,currentProperty,0);
    }

    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;
    tinyxml2::XMLElement *valueElt = propertyElt->FirstChildElement("value");
    uint32_t index = 0;
    while (valueElt != nullptr && result == XPCFErrorCode::_SUCCESS) {
        string propertyValue = valueElt->GetText();
        result = configureValue(propertyValue,currentProperty,index);
        index++;
        valueElt = valueElt->NextSiblingElement("value");
    }
    return result;

}

XPCFErrorCode configureElement(tinyxml2::XMLElement *element, SRef<IPropertyMap> nodeElement)
{
    if (!nodeElement) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }
    tinyxml2::XMLElement *propertyElt = element->FirstChildElement("property");
    while (propertyElt != nullptr) {
        configureProperty(propertyElt,nodeElement);
        propertyElt = propertyElt->NextSiblingElement("property");
    }
    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode PropertyManager::configure(std::function<bool(tinyxml2::XMLElement *)> xmlNodePredicate, const uuids::uuid & componentUUID, SRef<IConfigurable> componentRef,const char * filepath)
{
    //#1 load conf file
    //#2 find first configuration child node
    //#3 find matching component node
    //#4 map parameter to inner parameter : NOTE : what happens if new param exists in file but not in implementation ?
    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;
    tinyxml2::XMLDocument doc;
    //TODO: filepath must be wrapped in an utf8 observer !
    enum tinyxml2::XMLError loadOkay = doc.LoadFile(filepath);
    if (loadOkay == 0) {
        try {
#ifdef XPCF_WITH_LOGS
            BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Parsing XML from "<<filepath<<" config file";
            BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"NOTE : Command line arguments are overloaded with config file parameters";
#endif
            //TODO : check each element exists before using it !
            // a check should be performed upon announced module uuid and inner module uuid
            tinyxml2::XMLElement * rootElt = doc.RootElement();
            tinyxml2::XMLElement * xmlConfigElt = rootElt->FirstChildElement("configuration");
            tinyxml2::XMLElement * xmlPropsElt = rootElt->FirstChildElement("properties");
            if ((xmlConfigElt == nullptr) && (xmlPropsElt == nullptr)) {
                return XPCFErrorCode::_SUCCESS;
            }
            if (xmlConfigElt != nullptr) {
                tinyxml2::XMLElement * xmlComponentElt = xmlConfigElt->FirstChildElement("component");
                while (xmlComponentElt != nullptr && result == XPCFErrorCode::_SUCCESS) {
                    string uuid = xmlComponentElt->Attribute("uuid");
                    if (xmlNodePredicate(xmlComponentElt)) {
                        if (toUUID(uuid) == componentUUID) {
                            result = configureElement(xmlComponentElt, componentRef->getPropertyRootNode());
                        }
                    }
                    xmlComponentElt = xmlComponentElt->NextSiblingElement("component");
                }
            }
            if (xmlPropsElt != nullptr) {
                tinyxml2::XMLElement * xmlConfigureElt = xmlPropsElt->FirstChildElement("configure");
                while (xmlConfigureElt != nullptr && result == XPCFErrorCode::_SUCCESS) {
                    string componentAttrValue =  xmlConfigureElt->Attribute("component");
                    uuids::uuid uuid;
                    if (m_aliasManager->aliasExists(IAliasManager::Type::Component, componentAttrValue)) {
                        uuid = m_aliasManager->resolveComponentAlias(componentAttrValue);
                    }
                    else {
                        uuid =  toUUID(componentAttrValue);
                    }
                    if (xmlNodePredicate(xmlConfigureElt)) {
                        if (uuid == componentUUID) {
                            result = configureElement(xmlConfigureElt, componentRef->getPropertyRootNode());
                        }
                    }
                    xmlConfigureElt = xmlConfigureElt->NextSiblingElement("configure");
                }
            }
        }
        catch (const xpcf::Exception & e) {
            return e.getErrorCode();
        }
        catch (const std::runtime_error & e) {
#ifdef XPCF_WITH_LOGS
            BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"XML parsing file "<<filepath<<" failed with error : "<<e.what();
#endif
            return XPCFErrorCode::_FAIL;
        }
    }
    return result;
}

XPCFErrorCode PropertyManager::configure(const uuids::uuid & componentUUID, SRef<IConfigurable> componentRef, const char * filepath)
{
    std::function<bool(tinyxml2::XMLElement *)> unnamedPredicate = [](tinyxml2::XMLElement * element) {
        const char * name = element->Attribute("name");
        if (name == nullptr) {
                return true;
        }
        return false;
    };

    return configure(unnamedPredicate, componentUUID, componentRef, filepath);
}


XPCFErrorCode PropertyManager::configure(const char * instanceName, const uuids::uuid & componentUUID, SRef<IConfigurable> componentRef,const char * filepath)
{
    //#1 load conf file
    //#2 find first configuration child node
    //#3 find matching component node
    //#4 map parameter to inner parameter : NOTE : what happens if new param exists in file but not in implementation ?
    if (instanceName == nullptr) {
        return XPCFErrorCode::_ERROR_INVALID_ARGUMENT;
    }

    // NOTE : here we capture a const char pointer as the calling scope of the predicate is inside a function called within the method
    // Otherwise, we should have made a copy of the string to distinguish the string lifecycle from the pointer lifecycle
    std::function<bool(tinyxml2::XMLElement *)> namedPredicate = [instanceName](tinyxml2::XMLElement * element) {
        const char * name = element->Attribute("name");
        if (name != nullptr) {
            if (!strcmp(name, instanceName)) {
                return true;
            }
        }
        return false;
    };

    return configure(namedPredicate, componentUUID, componentRef, filepath);
}

void serializeParameter(tinyxml2::XMLDocument & xmlDoc, tinyxml2::XMLElement * nodeElement,SRef<xpcf::IProperty> p)
{
    std::cout<<"Property name ="<<p->getName()<<std::endl;
    tinyxml2::XMLElement * xmlPropertyElt = xmlDoc.NewElement("property");
    xmlPropertyElt->SetAttribute("name", p->getName());
    xmlPropertyElt->SetAttribute("access", propertyAccessToStrMap[p->getAccessSpecifier()].c_str());
    xmlPropertyElt->SetAttribute("type", propertyTypeToStrMap[p->getType()].c_str());
    for (uint32_t i=0 ; i < p->size() ; i++ ) {
        if (p->getType() == xpcf::IProperty::IProperty_STRUCTURE)            {
            SRef<xpcf::IPropertyMap> propertyMap = p->getStructureValue(i);
            std::cout<<"Accessing class values for C0 from IProperty/IPropertyMap interfaces"<<std::endl;
            for (auto property : propertyMap->getProperties()) {
                serializeParameter(xmlDoc, xmlPropertyElt, property);
            }
        }
        else {
            std::string value;
            switch (p->getType()) {
            case xpcf::IProperty::IProperty_NONE :
                break;
            case xpcf::IProperty::IProperty_INTEGER :
                value = to_string(p->getIntegerValue(i));

                std::cout<<"=> Int property = "<<p->getIntegerValue(i)<<std::endl;
                break;
            case xpcf::IProperty::IProperty_UINTEGER :
                value = to_string(p->getUnsignedIntegerValue(i));
                std::cout<<"=> Uint property = "<<p->getUnsignedIntegerValue(i)<<std::endl;
                break;
            case xpcf::IProperty::IProperty_LONG :
                value = to_string(p->getLongValue(i));
                std::cout<<"=> Long property = "<<p->getLongValue(i)<<std::endl;
                break;
            case xpcf::IProperty::IProperty_ULONG :
                value = to_string(p->getUnsignedLongValue(i));
                std::cout<<"=> ULong property = "<<p->getUnsignedLongValue(i)<<std::endl;
                break;
            case xpcf::IProperty::IProperty_CHARSTR :
                value = p->getStringValue(i);
                std::cout<<"=> String property = "<<p->getStringValue(i)<<std::endl;
                break;
            case xpcf::IProperty::IProperty_UNICODESTR :
                break;
            case xpcf::IProperty::IProperty_FLOAT :
                value = to_string(p->getFloatingValue(i));
                std::cout<<"=> Float property = "<<p->getFloatingValue(i)<<std::endl;
                break;
            case xpcf::IProperty::IProperty_DOUBLE :
                value = to_string(p->getDoubleValue(i));
                std::cout<<"=> Double property = "<<p->getDoubleValue(i)<<std::endl;
                break;
            default:
                break;
            }
            if (p->size() == 1) {
                xmlPropertyElt->SetAttribute("value",value.c_str());
            }
            else {
                tinyxml2::XMLElement * xmlValueElt = xmlDoc.NewElement("value");
                xmlValueElt->SetText(value.c_str());
                xmlPropertyElt->InsertEndChild(xmlValueElt);
            }
        }
    }
    nodeElement->InsertEndChild(xmlPropertyElt);
}

XPCFErrorCode PropertyManager::serialize(const uuids::uuid & componentUUID, SRef<IConfigurable> componentRef, const char * filepath, uint32_t mode)
{

    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;
    tinyxml2::XMLDocument xmlDoc;
    // root node creation will occur only for non existing files, otherwise the root node will be loaded and elements for the module will be inserted in the xml tree
    tinyxml2::XMLNode * xmlRoot;
    enum tinyxml2::XMLError loadOkay = xmlDoc.LoadFile(filepath);
    if (loadOkay == 0) {
        xmlRoot = xmlDoc.RootElement();
    }
    else {
        std::cout<<"ModuleManager::saveModuleInformations ===> failed loading doc "<<filepath<<std::endl;
        //TODO : add component registry file or folder path as an attribute
        xmlRoot = xmlDoc.NewElement("xpcf-configuration");
        xmlDoc.InsertFirstChild(xmlRoot);
    }
    try {
        //TODO : check config node existenz and create it only if no node already exists
        tinyxml2::XMLElement * xmlConfigurationElt = xmlDoc.NewElement("properties");
        tinyxml2::XMLElement * xmlComponentElt = xmlDoc.NewElement("configure");
        xmlComponentElt->SetAttribute("component", uuids::to_string(componentUUID).c_str());
        for (auto property : componentRef->getProperties()) {
            serializeParameter(xmlDoc, xmlComponentElt, property);
        }
        xmlConfigurationElt->InsertEndChild(xmlComponentElt);
        xmlRoot->InsertEndChild(xmlConfigurationElt);
        tinyxml2::XMLError eResult = xmlDoc.SaveFile(filepath);
        if (eResult != 0) {
            result = XPCFErrorCode::_FAIL;
        }
    }
    catch (const xpcf::Exception & e) {
        return e.getErrorCode();
    }
    catch (const std::runtime_error & e) {
#ifdef XPCF_WITH_LOGS
        BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"XML parsing file "<<filepath<<" failed with error : "<<e.what();
#endif
        return XPCFErrorCode::_FAIL;
    }
    return result;
}


}}} //namespace org::bcom::xpcf
