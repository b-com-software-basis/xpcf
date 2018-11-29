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
#include "xpcf/module/ModuleFactory.h"
#include "PropertyManager.h"
#include "PathBuilder.h"

#include "tinyxmlhelper.h"
#include <iostream>
#include <fstream>

using namespace std;

//namespace logging = boost::log;

namespace fs = boost::filesystem;

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

PropertyManager::PropertyManager():ComponentBase(toUUID<PropertyManager>())
{
    addInterface<IPropertyManager>(this);
    //  m_logger.add_attribute("ClassName", boost::log::attributes::constant<std::string>("PropertyManager"));
    //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Constructor PropertyManager::PropertyManager () called!";
}

void PropertyManager::unloadComponent ()
{
    //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"PropertyManager::unload () called!";
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

XPCFErrorCode configure(std::function<bool(tinyxml2::XMLElement *)> xmlNodePredicate, const uuids::uuid & componentUUID, SRef<IConfigurable> componentRef,const char * filepath)
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
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Parsing XML from "<<modulePath<<" config file";
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"NOTE : Command line arguments are overloaded with config file parameters";
            //TODO : check each element exists before using it !
            // a check should be performed upon announced module uuid and inner module uuid
            tinyxml2::XMLElement * rootElt = doc.RootElement();
            tinyxml2::XMLElement * xmlConfigElt = rootElt->FirstChildElement("configuration");
            if (xmlConfigElt == nullptr) {
                return XPCFErrorCode::_SUCCESS;
            }
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
        catch (const std::runtime_error & e) {
            //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"XML parsing file "<<modulePath<<" failed with error : "<<e.what();
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

    return org::bcom::xpcf::configure(unnamedPredicate, componentUUID, componentRef, filepath);
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

    return org::bcom::xpcf::configure(namedPredicate, componentUUID, componentRef, filepath);
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
        tinyxml2::XMLElement * xmlConfigurationElt = xmlDoc.NewElement("configuration");
        tinyxml2::XMLElement * xmlComponentElt = xmlDoc.NewElement("component");
        xmlComponentElt->SetAttribute("uuid", boost::uuids::to_string(componentUUID).c_str());
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
    catch (const std::runtime_error & e) {
        //BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"XML parsing file "<<modulePath<<" failed with error : "<<e.what();
        return XPCFErrorCode::_FAIL;
    }
    return result;
}


}}} //namespace org::bcom::xpcf
