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
#include "xpcf/core/ErrorMessage.h"
#include "xpcf/core/Exception.h"
#include "xpcf/module/ModuleFactory.h"
#include "private/xpcf/PropertyManager.h"
#include "private/xpcf/PathBuilder.h"

#include "private/xpcf/tinyxmlhelper.h"
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

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(org::bcom::xpcf::PropertyManager);

namespace org { namespace bcom { namespace xpcf {

constexpr const char * XMLCOMPONENTNODE = "component";
constexpr const char * XMLCONFIGURENODE = "configure";

void PropertyContext::clear()
{
    moduleConfigMap.clear();
    componentConfigMap.clear();
}

PropertyManager::PropertyManager():ComponentBase(toUUID<PropertyManager>())
{
    m_context = utils::make_shared<PropertyContext>();
    declareInterface<IPropertyManager>(this);
    declareInterface<AbstractPropertyManager>(this);
    declareInjectable<IAliasManager>(m_aliasManager);
    declareInjectable<IRegistryManager>(m_registry);
#ifdef XPCF_WITH_LOGS
    m_logger.add_attribute("ClassName", boost::log::attributes::constant<std::string>("PropertyManager"));
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Constructor PropertyManager::PropertyManager () called!";
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
    m_context->clear();
}

SRef<PropertyContext> PropertyManager::getContext() const
{
    return m_context;
}

void PropertyManager::setContext(SRef<PropertyContext> context)
{
    m_context = context;
}

void PropertyManager::declareComponent(tinyxml2::XMLElement * xmlElt, const fs::path & configFilePath)
{
    bool subElementError = false;
    string componentUuidStr = "";
    try {
        if(xmlElt->Attribute("uuid") != nullptr) {
            componentUuidStr = xmlElt->Attribute("uuid");
        }
        uuids::uuid componentUuid;
        try {
            componentUuid = toUUID(componentUuidStr);
        }
        catch(const std::runtime_error &) {
            std::string what = "UUID format invalid in \"uuid\": ";
            what.append(xmlElt->Attribute("uuid"));
            throw ConfigurationException( what );
        }
        try {
            uuids::uuid moduleUuid = m_registry->getModuleUUID(componentUuid);
            if (m_context->moduleConfigMap.find(moduleUuid) == m_context->moduleConfigMap.end()) {
                m_context->moduleConfigMap[moduleUuid] = configFilePath;
            }
            else {
                fs::path & moduleConfigurationPath = m_context->moduleConfigMap[moduleUuid];
                if (moduleConfigurationPath != configFilePath) {
                    m_context->componentConfigMap[componentUuid] = configFilePath;
                }
            }
        }
        catch (const ModuleNotFoundException & e) {
            // no associated module in current configuration :
            // add config path to component configuration as the component can be declared from bindLocal
            m_context->componentConfigMap[componentUuid] = configFilePath;
        }
    }
    catch (const xpcf::Exception& e) {
        XmlErrorMessage errMsg("component", e.what(), subElementError);
        errMsg.addAttribute("uuid", componentUuidStr);
        throw ConfigurationException(errMsg.write());
    }
}

void PropertyManager::declareConfigure(tinyxml2::XMLElement * xmlElt, const fs::path & configFilePath)
{
    string componentAttrValue = "";
    try {
        if(xmlElt->Attribute("component") != nullptr) {
            componentAttrValue = xmlElt->Attribute("component");
        }
        uuids::uuid componentUuid;

        if (m_aliasManager->aliasExists(IAliasManager::Type::Component, componentAttrValue)) {
            componentUuid = m_aliasManager->resolveComponentAlias(componentAttrValue);
        }
        else {
            try {
                componentUuid =  toUUID(componentAttrValue);
            }
            catch(const std::runtime_error &) {
                std::string what = "UUID format invalid in \"component\": ";
                what.append(componentAttrValue);
                throw ConfigurationException( what );
            }
        }
        try {
            uuids::uuid moduleUuid = m_registry->getModuleUUID(componentUuid);
            if (m_context->moduleConfigMap.find(moduleUuid) == m_context->moduleConfigMap.end()) {
                m_context->moduleConfigMap[moduleUuid] = configFilePath;
            }
            else {
                fs::path & moduleConfigurationPath = m_context->moduleConfigMap[moduleUuid];
                if (moduleConfigurationPath != configFilePath) {
                    m_context->componentConfigMap[componentUuid] = configFilePath;
                }
            }
        }
        catch (const ModuleNotFoundException & e) {
            // no associated module in current configuration :
            // add config path to component configuration as the component can be declared from bindLocal
            m_context->componentConfigMap[componentUuid] = configFilePath;
        }
    }
    catch (const xpcf::Exception& e) {
        XmlErrorMessage errMsg("configure", e.what(), false);
        errMsg.addAttribute("component", componentAttrValue);
        throw ConfigurationException(errMsg.write());
    }
}

void PropertyManager::declareConfiguration(tinyxml2::XMLElement * xmlElt, const fs::path & configFilePath)
{
    try {
        std::function<void(tinyxml2::XMLElement*,  const fs::path &)> declareComponentFunc = std::bind(&PropertyManager::declareComponent, this, _1,_2);
        processXmlNode<const fs::path &>(xmlElt, XMLCOMPONENTNODE, declareComponentFunc, configFilePath);
    }
    catch(const xpcf::Exception& e) {
        XmlErrorMessage errMsg("configuration", e.what(), true);
        throw ConfigurationException(errMsg.write());
    }
}

void PropertyManager::declareProperties(tinyxml2::XMLElement * xmlElt, const fs::path & configFilePath)
{
    try {
        std::function<void(tinyxml2::XMLElement*,  const fs::path &)> declareConfigureFunc = std::bind(&PropertyManager::declareConfigure, this, _1,_2);
        processXmlNode<const fs::path &>(xmlElt, XMLCONFIGURENODE, declareConfigureFunc, configFilePath);
    }
    catch (const xpcf::Exception& e) {
        XmlErrorMessage errMsg("properties", e.what(), true);
        throw ConfigurationException(errMsg.write());
    }
}

fs::path PropertyManager::getConfigPath(uuids::uuid componentUUID) const
{
    if (m_context->componentConfigMap.find(componentUUID) != m_context->componentConfigMap.end()) {
        return m_context->componentConfigMap.at(componentUUID);
    }
    try {
        uuids::uuid moduleUUID = m_registry->getModuleUUID(componentUUID);
        if (m_context->moduleConfigMap.find(moduleUUID) == m_context->moduleConfigMap.end()) {
            //log("No configuration path found for module "+uuids::to_string(moduleUUID));
            // return empty path
            return fs::path();
        }
        return m_context->moduleConfigMap.at(moduleUUID);
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
        string what = "No value defined";
        throw ConfigurationException(what);
    }

    try {
        size_t processedValue = 0;

        switch (property->getType()) {
        case IProperty::IProperty_INTEGER:
            property->setIntegerValue(std::stol(valueStr,&processedValue,0), valueIndex);
            break;
        case IProperty::IProperty_UINTEGER:
            property->setUnsignedIntegerValue(std::stoul(valueStr,&processedValue,0), valueIndex);
            break;
        case IProperty::IProperty_LONG:
            property->setLongValue(std::stoll(valueStr,&processedValue,0), valueIndex);
            break;
        case IProperty::IProperty_ULONG:
            property->setUnsignedLongValue(std::stoull(valueStr,&processedValue,0), valueIndex);
            break;
        case IProperty::IProperty_FLOAT:
            property->setFloatingValue(std::stof(valueStr,&processedValue), valueIndex);
            break;
        case IProperty::IProperty_DOUBLE:
            property->setDoubleValue(std::stod(valueStr,&processedValue), valueIndex);
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

        if((valueStr.size() != processedValue)&&(property->getType()!=IProperty::IProperty_CHARSTR)) {
            std::string what = "Unexpected value type for \"";
            what.append( valueStr );
            what.append("\", \"");
            what.append(propertyTypeToStrMap[property->getType()]);
            what.append("\" expected");
            throw ConfigurationException(what);
        }
    }
    catch (const std::invalid_argument & e) {
        string what = "Unexpected value type for \"";
        what.append( valueStr );
        what.append("\", \"");
        what.append(propertyTypeToStrMap[property->getType()]);
        what.append("\" expected");
        throw ConfigurationException(what);
    }
    return XPCFErrorCode::_SUCCESS;
}

XPCFErrorCode configureElement(tinyxml2::XMLElement *element, SRef<IPropertyMap> nodeElement);

XPCFErrorCode configureProperty(tinyxml2::XMLElement *propertyElt, SRef<IPropertyMap> nodeElement)
{
    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;

    string propertyName = "";
    if(propertyElt->Attribute("name") != nullptr) {
        propertyName = propertyElt->Attribute("name");
    }
    if ( propertyName.empty() ) {
        return XPCFErrorCode::_ERROR_INVALID_ARGUMENT;
    }
    
    try {
        SRef<IProperty> currentProperty = nodeElement->at(propertyName.c_str());
        if (currentProperty == nullptr) {
            std::string what = "Property unknown: ";
            what.append(propertyName.c_str());
            throw ConfigurationException(what);
        }

        string propertyType = propertyElt->Attribute("type");
        if ( propertyType.empty() ) {
            string what = "Unknown type \"";
            what.append(propertyElt->Attribute("type"));
            what.append("\"");
            throw ConfigurationException(what);
        }

        if (propertyType == "structure") {
            if (currentProperty->getType() != IProperty::IProperty_STRUCTURE) {
                string what = "\"structure\" expected.";
                throw ConfigurationException(what);
            }

            SRef<IPropertyMap> propertyNode = currentProperty->getStructureValue();
            return configureElement(propertyElt,propertyNode);
        }

        const char * propertyCharStr = propertyElt->Attribute("value");
        if (propertyCharStr != nullptr) {
            string propertyValue = propertyCharStr;
            return configureValue(propertyValue,currentProperty,0);
        }

        tinyxml2::XMLElement *valueElt = propertyElt->FirstChildElement("value");
        uint32_t index = 0;
        while (valueElt != nullptr && result == XPCFErrorCode::_SUCCESS) {
            string propertyValue = valueElt->GetText();
            result = configureValue(propertyValue,currentProperty,index);
            index++;
            valueElt = valueElt->NextSiblingElement("value");
        }
    } 
    catch  (const xpcf::Exception & e) {
        XmlErrorMessage errMsg("property", e.what(), false);
        errMsg.addAttribute("name", propertyName);
        throw ConfigurationException(errMsg.write());
    }

    return result;
}

XPCFErrorCode configureElement(tinyxml2::XMLElement *element, SRef<IPropertyMap> nodeElement)
{
    if (!nodeElement) {
        return XPCFErrorCode::_ERROR_NULL_POINTER;
    }
    tinyxml2::XMLElement *propertyElt = element->FirstChildElement("property");
    XPCFErrorCode result = XPCFErrorCode::_SUCCESS;
    while (propertyElt != nullptr && result == XPCFErrorCode::_SUCCESS) {
        result = configureProperty(propertyElt,nodeElement);
        propertyElt = propertyElt->NextSiblingElement("property");
    }
    return result;
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
            try {
                try {
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
                catch (const xpcf::Exception & e) {
                    XmlErrorMessage errMsg("component", e.what(), true);
                    if(xmlComponentElt->Attribute("uuid") != nullptr) {
                        errMsg.addAttribute("uuid", xmlComponentElt->Attribute("uuid"));
                    }
                    if(xmlComponentElt->Attribute("name") != nullptr) {
                        errMsg.addAttribute("name", xmlComponentElt->Attribute("name"));
                    }
                    throw ConfigurationException(errMsg.write());
                }
                catch (const std::runtime_error & e) {
                    string what = "UUID format invalid in \"uuid\": ";
                    what.append(xmlComponentElt->Attribute("uuid"));
                    XmlErrorMessage errMsg("component", what, false);
                    errMsg.addAttribute("name", xmlComponentElt->Attribute("name"));
                    throw ConfigurationException(errMsg.write());
                }
            }
            catch (const xpcf::Exception & e) {
                XmlErrorMessage errMsg("configuration", e.what(), true);
                throw ConfigurationException(errMsg.write());
            }
        }
        if (xmlPropsElt != nullptr) {
            try {
                tinyxml2::XMLElement * xmlConfigureElt = xmlPropsElt->FirstChildElement("configure");
                try {
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
                catch (const xpcf::Exception & e) {
                    XmlErrorMessage errMsg("configure", e.what(), true);
                    errMsg.addAttribute("component", xmlConfigureElt->Attribute("component"));
                    throw ConfigurationException(errMsg.write());
                }
                catch (const std::runtime_error & e) {
                    string what = "UUID format invalid in \"component\":";
                    what.append(xmlConfigureElt->Attribute("component"));
                    XmlErrorMessage errMsg("configure", what, false);
                    throw ConfigurationException(errMsg.write());
                }
            }
            catch (const std::runtime_error & e) {
                XmlErrorMessage errMsg("properties", e.what(), true);
                throw ConfigurationException(errMsg.write());
            }
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
                break;
            case xpcf::IProperty::IProperty_UINTEGER :
                value = to_string(p->getUnsignedIntegerValue(i));
                break;
            case xpcf::IProperty::IProperty_LONG :
                value = to_string(p->getLongValue(i));
                break;
            case xpcf::IProperty::IProperty_ULONG :
                value = to_string(p->getUnsignedLongValue(i));
                break;
            case xpcf::IProperty::IProperty_CHARSTR :
                value = p->getStringValue(i);
                break;
            case xpcf::IProperty::IProperty_UNICODESTR :
                break;
            case xpcf::IProperty::IProperty_FLOAT :
                value = to_string(p->getFloatingValue(i));
                break;
            case xpcf::IProperty::IProperty_DOUBLE :
                value = to_string(p->getDoubleValue(i));
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
