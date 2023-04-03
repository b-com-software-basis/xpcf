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
 * @date 2019-06-05
 */


#include "private/xpcf/Factory.h"
#include <xpcf/api/IModuleManager.h>
#include <xpcf/core/ErrorMessage.h>
#include <xpcf/core/Exception.h>
#include <xpcf/core/helpers.h>
#include <boost/algorithm/string.hpp>
#include "private/xpcf/PathBuilder.h"

using namespace std;
using placeholders::_1;
using placeholders::_2;

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(org::bcom::xpcf::Factory);

#ifdef XPCF_WITH_LOGS
namespace logging = boost::log;
#endif

namespace org { namespace bcom { namespace xpcf {

constexpr const char * XMLBINDINGSNODE = "bindings";
constexpr const char * XMLBINDNODE = "bind";
constexpr const char * XMLINJECTSNODE = "injects";
constexpr const char * XMLINJECTNODE = "inject";
constexpr const char * XMLSCOPENODE = "scope";

static const map<string,BindingScope> scopeConvertMap = {
    {"Transient",BindingScope::Transient},
    {"Singleton",BindingScope::Singleton},
};

static const map<string,BindingRange> bindingRangeConvertMap = {
    {"all",BindingRange::All},
    {"default",BindingRange::Default},
    {"explicit",BindingRange::Explicit},
    {"named",BindingRange::Named},
    {"withparents",BindingRange::WithParents}
};

void FactoryContext::clear()
{
    autoBindings.clear();
    defaultBindings.clear();
    multiBindings.clear();
    namedBindings.clear();
    namedBindingsProperties.clear();
    bindingsProperties.clear();
    multiBindingsProperties.clear();
    factoryMethods.clear();
    specificBindings.clear();
    specificNamedBindings.clear();
}

Factory::Factory():ComponentBase(toUUID<Factory>())
{
    // bind structures must exist prior to Factory inner binds
    m_context = utils::make_shared<FactoryContext>() ;

    declareInterface<IFactory>(this);
    declareInterface<AbstractFactory>(this);
    declareInterface<IAliasManager>(this);
    declareInterface<IRegistryManager>(this);

    Factory::bindCore<IAliasManager,AliasManager,BindingScope::Singleton>();
    Factory::bindCore<IRegistryManager,Registry,BindingScope::Singleton>();
    Factory::bindCore<IPropertyManager,PropertyManager,BindingScope::Singleton>();
    m_propertyManager = IFactory::resolve<IPropertyManager>()->bindTo<AbstractPropertyManager>();
    m_aliasManager = IFactory::resolve<IAliasManager>()->bindTo<AbstractAliasManager>();
    m_resolver = IFactory::resolve<IRegistryManager>()->bindTo<AbstractRegistry>();
    std::function<void(const uuids::uuid &, const uuids::uuid &)> bindFunc = [&] (const uuids::uuid & interfaceUUID, const uuids::uuid & componentUUID) -> void {
        autobind(interfaceUUID,componentUUID);
    };
    m_resolver->bindTo<AbstractRegistry>()->setBinder(bindFunc);
#ifdef XPCF_WITH_LOGS
    m_logger.add_attribute("ClassName", boost::log::attributes::constant<std::string>("Factory"));
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Constructor Factory::Factory () called!";
#endif
}

#ifdef XPCF_WITH_LOGS
std::stringstream Factory::log(const FactoryBindInfos & bindInfos)
{
    std::string scope,range;
    range = "|";
    scope = "Transient";
    if (bindInfos.bindingRangeMask & BindingRange::Explicit) {
        range += " Explicit |";
    }
    if (bindInfos.bindingRangeMask & BindingRange::WithParents) {
        range += " WithParents |";
    }
    if (bindInfos.bindingRangeMask & BindingRange::Named) {
        range += " Named |";
    }
    if (bindInfos.bindingRangeMask & BindingRange::Default) {
        range += " Default |";
    }
    if (bindInfos.bindingRangeMask & BindingRange::All) {
        range += " All |";
    }
    if (bindInfos.scope == BindingScope::Singleton) {
        scope = "Singleton";
    }
    std::stringstream result;
    result << "Bind infos:"<<std::endl;
    result << "=> component uuid= '" << uuids::to_string(bindInfos.componentUUID)<<"'"<<std::endl;
    result << "=> scope         = "<<scope<<std::endl;
    result << "=> range         = "<<range<<std::endl;
    result << "=> properties    = "<<bindInfos.properties<<std::endl;
    return result;
}
#endif
// the context can be shared from xpcf main factory, or dedicated to this new factory context and in this case empty
// however, if the context is empty, how can the user populate the registry, aliases, props as the factory has no load method and for the moment doesn't provide access to its inner components ..?
SRef<IFactory> Factory::createNewFactoryContext(ContextMode ctxMode)
{
#ifdef XPCF_WITH_LOGS
    std::string mode;
    switch (ctxMode) {
    case ContextMode::Empty : mode = "empty";
        break;
    case ContextMode::Cloned : mode = "cloned";
        break;
    case ContextMode::Shared : mode = "shared";
        break;
    default:
        break;
    }
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Factory::createNewFactoryContext with mode="<<mode;
#endif
    Factory * f = new Factory();
    if (ctxMode == ContextMode::Cloned) {
        // TODO clone alias, props informations
        // for the moment, the propertymanager & alias mgr are shared between factories when context is cloned.
        *(f->m_propertyManager->context()) = *(m_propertyManager->getContext());
        *(f->m_aliasManager->context()) = *(m_aliasManager->getContext());
        *(f->m_resolver->context()) = *(m_resolver->getContext());
        *(f->m_context) = *m_context;
    }
    if (ctxMode == ContextMode::Shared) {
        f->m_propertyManager->setContext(m_propertyManager->getContext());
        f->m_aliasManager->setContext(m_aliasManager->getContext());
        f->m_resolver->setContext(m_resolver->getContext());

        f->m_context = m_context;
    }
    return IComponentIntrospect::acquireComponentRef<Factory, IComponentIntrospect>(f)->bindTo<IFactory>();
}

void Factory::clear()
{
    m_context->clear();
    m_singletonInstances.clear();
    m_namedSingletonInstances.clear();
    m_propertyManager->clear();
    m_aliasManager->clear();
    m_resolver->clear();
}

SRef<IComponentIntrospect> Factory::create(const uuids::uuid & componentUUID)
{
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"ComponentManager::create uuid="<<uuids::to_string(componentUUID);
#endif
    SPtr<ModuleMetadata> moduleInfos = findModuleMetadata(getModuleUUID(componentUUID));
    SRef<IComponentIntrospect> componentRef = getModuleManagerInstance()->createComponent(moduleInfos, componentUUID);
    return componentRef;
}

SRef<IComponentIntrospect> Factory::createComponent(const uuids::uuid & componentUUID)
{
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"ComponentManager::createComponent uuid="<<uuids::to_string(componentUUID);
#endif
    SRef<IComponentIntrospect> componentRef = create(componentUUID);
    inject(componentRef->bindTo<IInjectable>());

    fs::path configFilePath = m_propertyManager->getConfigPath(componentUUID);
    if (componentRef->implements<IConfigurable>() && ! configFilePath.empty()) {
        SRef<IConfigurable> iconf = componentRef->bindTo<IConfigurable>();
        iconf->configure(configFilePath.string().c_str());
    }
    return componentRef;
}

SRef<IComponentIntrospect> Factory::createComponent(const char * instanceName, const uuids::uuid & componentUUID)
{
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"ComponentManager::createComponent name="<<instanceName<<" uuid="<<uuids::to_string(componentUUID);
#endif
    SRef<IComponentIntrospect> componentRef = create(componentUUID);
    inject(componentRef->bindTo<IInjectable>());

    fs::path configFilePath = m_propertyManager->getConfigPath(componentUUID);
    if (componentRef->implements<IConfigurable>() && ! configFilePath.empty()) {
        SRef<IConfigurable> iconf = componentRef->bindTo<IConfigurable>();
        iconf->configure(configFilePath.string().c_str(), instanceName);
    }
    return componentRef;
}

XPCFErrorCode Factory::load()
{
    const char * registryPath = getenv( "XPCF_REGISTRY_PATH" );
    if (registryPath != nullptr) {
        load(registryPath);
    }
    return load<fs::recursive_directory_iterator>(PathBuilder::getXPCFHomePath());
}

XPCFErrorCode Factory::load(const char* libraryFilePath)
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

template <class T> XPCFErrorCode Factory::load(fs::path folderPath)
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

XPCFErrorCode Factory::load(const char* folderPathStr, bool bRecurse)
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
    return XPCFErrorCode::_SUCCESS;
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
XPCFErrorCode Factory::loadLibrary(fs::path configurationFilePath)
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
        std::string what = "configuration loading failed: ";
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
                throw ConfigurationException("missing root element \"xpcf-registry\" or \"xpcf-configuration\"", XPCFErrorCode::_ERROR_RANGE);
            }
            result = XPCFErrorCode::_SUCCESS;
            const char * autoAliasProp = rootElt->Attribute("autoAlias");
            m_resolver->enableAutoAlias(false);
            if (autoAliasProp != nullptr) {
                const string autoAliasStr(autoAliasProp);
                if (autoAliasStr == "true") {
                    m_resolver->enableAutoAlias(true);
                }
            }
            processXmlNode(rootElt, "module", std::bind(&AbstractRegistry::declareModule, m_resolver->bindTo<AbstractRegistry>().get(), _1));
            processXmlNode(rootElt, "aliases", std::bind(&AbstractAliasManager::declareAliases, m_aliasManager->bindTo<AbstractAliasManager>().get(), _1));
            processXmlNode(rootElt, "factory", std::bind(&AbstractFactory::declareFactory, this, _1));
            std::function<void(tinyxml2::XMLElement*,  const fs::path &)> declareConfigureFunc = std::bind(&AbstractPropertyManager::declareConfiguration,  m_propertyManager->bindTo<AbstractPropertyManager>().get(), _1,_2);
            processXmlNode<const fs::path &>(rootElt, "configuration", declareConfigureFunc, configurationFilePath);
            std::function<void(tinyxml2::XMLElement*,  const fs::path &)> declarePropertiesFunc = std::bind(&AbstractPropertyManager::declareProperties,  m_propertyManager->bindTo<AbstractPropertyManager>().get(), _1,_2);
            processXmlNode<const fs::path &>(rootElt, "properties", declarePropertiesFunc, configurationFilePath);
        }
        catch (const xpcf::Exception & e) {
            what.append(e.what());
            throw ConfigurationException(what);
        }
        catch (const std::runtime_error & e) {
#ifdef XPCF_WITH_LOGS
            BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"XML parsing file "<<configurationFilePath<<" failed with error : "<<e.what();
#endif
            return XPCFErrorCode::_FAIL;
        }
    }
    else
    {
        throw ConfigurationException("Configuration failed, missing end tag: ", XPCFErrorCode::_FAIL);
    }
    return result;
}

FactoryBindInfos Factory::getComponentBindingInfos(tinyxml2::XMLElement * xmlBindElt)
{
    FactoryBindInfos infos;
    // Set default scope to Transient
    infos.scope = BindingScope::Transient;
    infos.bindingRangeMask = 0;
    string componentAttrValue =  xmlBindElt->Attribute("to");
    if (m_aliasManager->aliasExists(IAliasManager::Type::Component, componentAttrValue)) {
        infos.componentUUID = m_aliasManager->resolveComponentAlias(componentAttrValue);
    }
    else {
        try {
            infos.componentUUID = toUUID(componentAttrValue);
        }
        catch(const std::exception& ) {
            std::string what = "UUID format invalid in \"to\" attribute";
            what.append(xmlBindElt->Attribute("to"));
            throw ConfigurationException(what);
        }
    }
    const char * scopeType =  xmlBindElt->Attribute("scope");
    if (scopeType != nullptr) {
        infos.scope = scopeConvertMap.at(scopeType);
    }
    const char * propertiesName =  xmlBindElt->Attribute("properties");
    if (propertiesName != nullptr) {
        infos.properties = propertiesName;
    }
    const char * rangeAttribute =  xmlBindElt->Attribute("range");
    if (rangeAttribute != nullptr) {
        std::vector<std::string> ranges;
        boost::split(ranges, rangeAttribute, [](char c){return c == '|';});
        for (auto& range: ranges) {
            infos.bindingRangeMask = infos.bindingRangeMask | bindingRangeConvertMap.at(range);
        }
    }
    return infos;
}

void Factory::declareSingleBind(const uuids::uuid & interfaceUUID, tinyxml2::XMLElement * xmlBindElt)
{
    FactoryBindInfos infos = getComponentBindingInfos(xmlBindElt);

    if (xmlBindElt->Attribute("name") == nullptr) {
        infos.bindingRangeMask |= BindingRange::Default;
        bind(interfaceUUID, infos);
    }
    else {
        infos.bindingRangeMask |= BindingRange::Named;
        string nameAttrValue =  xmlBindElt->Attribute("name");
        bind(nameAttrValue, interfaceUUID, infos);
    }
}

void Factory::declareMultiBind(const uuids::uuid & interfaceUUID, tinyxml2::XMLElement * xmlBindElt)
{
    tinyxml2::XMLElement *element = xmlBindElt->FirstChildElement("component");
    VectorCollection<FactoryBindInfos> & binds = m_context->multiBindings[interfaceUUID];
    while (element != nullptr) {
        FactoryBindInfos infos = getComponentBindingInfos(element);
        infos.bindingRangeMask |= BindingRange::Default;
        binds.add(infos);
        element = element->NextSiblingElement("component");
    }
}

void Factory::declareBind(tinyxml2::XMLElement * xmlBindElt)
{
    bool subElementError = false;
    string interfaceAttrValue = "";
    try {
        if (xmlBindElt->Attribute("interface") == nullptr) {
            throw Exception("node incomplete : attribute 'interface' is mandatory.");
        }
        interfaceAttrValue = xmlBindElt->Attribute("interface");
        uuids::uuid interfaceUUID;
        if (m_aliasManager->aliasExists(IAliasManager::Type::Interface, interfaceAttrValue)) {
            interfaceUUID = m_aliasManager->resolveInterfaceAlias(interfaceAttrValue);
        }
        else {
            try {
                interfaceUUID = toUUID(interfaceAttrValue);
            }
            catch(const std::runtime_error& e) {
                std::string what = "UUID format invalid in \"interface\" :";
                what.append(xmlBindElt->Attribute("interface"));
                throw ConfigurationException(what);
            }
        }
        subElementError = true;

        tinyxml2::XMLElement * xmlComponentNode = xmlBindElt->FirstChildElement("component");
        if ((xmlBindElt->Attribute("to") == nullptr) && (xmlComponentNode == nullptr)) {
            throw Exception("node incomplete : attributes 'to' or 'component' inner node are mandatory.");
        }

        if (xmlBindElt->Attribute("to") != nullptr) {
            declareSingleBind(interfaceUUID, xmlBindElt);
        }

        if (xmlComponentNode != nullptr) {
            declareMultiBind(interfaceUUID, xmlBindElt);
        }
    }
    catch (xpcf::Exception& e) {
        XmlErrorMessage errMsg("bind", e.what(), subElementError);
        if(!interfaceAttrValue.empty()) {
            errMsg.addAttribute("interface", interfaceAttrValue);
        }
        throw ConfigurationException(errMsg.write());
    }
}

void Factory::declareSpecificBind(tinyxml2::XMLElement * xmlBindElt, const uuids::uuid & targetComponentUUID)
{
    string interfaceAttrValue = "";
    try {
        if ((xmlBindElt->Attribute("interface") == nullptr) || (xmlBindElt->Attribute("to") == nullptr)) {
            throw Exception("node incomplete : attributes 'interface' and 'to' are mandatory.");
        }
        interfaceAttrValue =  xmlBindElt->Attribute("interface");
        uuids::uuid interfaceUUID;
        if (m_aliasManager->aliasExists(IAliasManager::Type::Interface, interfaceAttrValue)) {
            interfaceUUID = m_aliasManager->resolveInterfaceAlias(interfaceAttrValue);
        }
        else {
            try {
                interfaceUUID = toUUID(interfaceAttrValue);
            }
            catch(const std::exception& ) {
                std::string what = "UUID format invalid in \"interface\": ";
                what.append(xmlBindElt->Attribute("interface"));
                throw ConfigurationException(what);
            }
        }

        FactoryBindInfos infos = getComponentBindingInfos(xmlBindElt);
        infos.bindingRangeMask |= BindingRange::Explicit;
        if (xmlBindElt->Attribute("name") == nullptr) {
            bind(targetComponentUUID, interfaceUUID, infos);
        }
        else {
            string nameAttrValue =  xmlBindElt->Attribute("name");
            bind(targetComponentUUID, nameAttrValue, interfaceUUID, infos);
        }
    }
    catch (const xpcf::Exception& e) {
        XmlErrorMessage errMsg("bind", e.what(), false);
        if(!interfaceAttrValue.empty()) {
            errMsg.addAttribute("interface", interfaceAttrValue);
        }
        throw ConfigurationException(errMsg.write());
    }
}

void Factory::declareBindings(tinyxml2::XMLElement * xmlBindingsElt)
{
    try {
        processXmlNode(xmlBindingsElt, XMLBINDNODE, std::bind(&Factory::declareBind, this, _1));
    }
    catch (const xpcf::Exception& e) {
        XmlErrorMessage errMsg("bindings", e.what(), true);
        throw ConfigurationException(errMsg.write());
    }
}

void Factory::declareInject(tinyxml2::XMLElement * xmlInjectElt)
{
    bool subElementError = false;
    string componentAttrValue = "";
    try {
        // Parse bindings : if binding already exist in default or namedBindings: ignore it
        // for each non-existent bind, add the specificBinding
        if (xmlInjectElt->Attribute("to") == nullptr) {
            throw Exception("node incomplete : attribute 'to' is mandatory.");
        }
        uuids::uuid componentUUID;
        componentAttrValue =  xmlInjectElt->Attribute("to");
        if (m_aliasManager->aliasExists(IAliasManager::Type::Component, componentAttrValue)) {
            componentUUID = m_aliasManager->resolveComponentAlias(componentAttrValue);
        }
        else {
            try {
                componentUUID =  toUUID(componentAttrValue);
            }
            catch(const std::exception& ) {
                std::string what = "UUID format invalid in \"to\": ";
                what.append(xmlInjectElt->Attribute("to"));
                throw ConfigurationException(what);
            }
        }
        subElementError = true;
        std::function<void(tinyxml2::XMLElement*,  const uuids::uuid &)> declareSpecificBindingsFunc = std::bind(&Factory::declareSpecificBind, this, _1,_2);
        processXmlNode<const uuids::uuid &>(xmlInjectElt, XMLBINDNODE, declareSpecificBindingsFunc, componentUUID);
    }
    catch (const xpcf::Exception& e) {
        XmlErrorMessage errMsg("inject", e.what(), subElementError);
        if(!componentAttrValue.empty()) {
            errMsg.addAttribute("to", componentAttrValue);
        }
        throw ConfigurationException(errMsg.write());
    }
}

void Factory::declareInjects(tinyxml2::XMLElement * xmlInjectsElt)
{
    try {
        processXmlNode(xmlInjectsElt, XMLINJECTNODE, std::bind(&Factory::declareInject, this, _1));
    }
    catch (const xpcf::Exception& e) {
        XmlErrorMessage errMsg("injects", e.what(), true);
        throw ConfigurationException(errMsg.write());
    }
}

void Factory::declareFactory(tinyxml2::XMLElement * xmlFactoryElt)
{
    try {
        processXmlNode(xmlFactoryElt, XMLBINDINGSNODE, std::bind(&Factory::declareBindings, this, _1));
        processXmlNode(xmlFactoryElt, XMLINJECTSNODE, std::bind(&Factory::declareInjects, this, _1));
    }
    catch (const xpcf::Exception& e) {
        XmlErrorMessage errMsg("factory", e.what(), true);
        throw ConfigurationException(errMsg.write());
    }
}

SRef<IComponentIntrospect> Factory::resolveFromModule(const uuids::uuid & componentUUID)
{
    SPtr<ModuleMetadata> moduleInfos = resolveMetadataFromComponentUUID(componentUUID);
    SRef<IComponentIntrospect> componentRef = getModuleManagerInstance()->createComponent(moduleInfos, componentUUID);
    return componentRef;
}

void Factory::autobind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID)
{
    if (! mapContains(m_context->defaultBindings,interfaceUUID)) {
        // no explicit bind already exists : add autobind
        m_context->autoBindings[interfaceUUID] = FactoryBindInfos{instanceUUID, BindingScope::Transient, BindingRange::All, ""};
    }
}

void Factory::bindCore(const uuids::uuid & interfaceUUID, const FactoryBindInfos & bindInfos)
{
    if (mapContains(m_coreBindings,interfaceUUID)) {
        // bind already exists : error ???
        // should we return or update the bind ?
    }
    m_coreBindings[interfaceUUID] = bindInfos;
}

template < typename I, typename C, BindingScope scope> void Factory::bindCore()
{
    uuids::uuid componentUUID = toUUID<C>();
    bindCore(toUUID<I>(),FactoryBindInfos{ componentUUID, scope, BindingRange_Core, ""});
    m_coreFactoryMethods[componentUUID] = &ComponentFactory::create<C>;
}

void Factory::bind(const uuids::uuid & interfaceUUID, const FactoryBindInfos & bindInfos)
{
    if (mapContains(m_context->defaultBindings,interfaceUUID)) {
        // bind already exists : error ???
        // should we return or update the bind ?
    }
    if (mapContains(m_context->autoBindings,interfaceUUID)) {
        // remove autobind as an explicit bind is added
        m_context->autoBindings.erase(interfaceUUID);
    }
    m_context->defaultBindings[interfaceUUID] = bindInfos;
}

void Factory::bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                   BindingScope scope,
                   uint8_t bindingRangeMask)
{
    bind(interfaceUUID, FactoryBindInfos{instanceUUID, scope, bindingRangeMask, ""});
}


void Factory::bind(const std::string & name, const uuids::uuid & interfaceUUID,
                   const FactoryBindInfos & bindInfos)
{
    pair<uuids::uuid,string> key = make_pair(interfaceUUID,name);
    if (mapContains(m_context->namedBindings,key)) {
        // bind already exists : error ???
    }
    m_context->namedBindings[key] = bindInfos;
}

void Factory::bind(const std::string & name, const uuids::uuid & interfaceUUID,
                   const uuids::uuid & instanceUUID, BindingScope scope,
                   uint8_t bindingRangeMask)
{
    bind(name, interfaceUUID, FactoryBindInfos{instanceUUID, scope, bindingRangeMask, ""});
}

void Factory::bind(const uuids::uuid & interfaceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   const FactoryBindInfos & bindInfos)
{
    bind(interfaceUUID, bindInfos);
    m_context->factoryMethods[bindInfos.componentUUID] = factoryFunc;
}

void Factory::bind(const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   BindingScope scope,
                   uint8_t bindingRangeMask)
{
    bind(interfaceUUID, factoryFunc, FactoryBindInfos{instanceUUID, scope, bindingRangeMask, ""});
}

void Factory::bind(const std::string & name, const uuids::uuid & interfaceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   const FactoryBindInfos & bindInfos)
{
    bind(name, interfaceUUID, bindInfos);
    m_context->factoryMethods[bindInfos.componentUUID] = factoryFunc;
}

void Factory::bind(const std::string & name, const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   BindingScope scope,
                   uint8_t bindingRangeMask)
{
    bind(name, interfaceUUID, factoryFunc, FactoryBindInfos{instanceUUID, scope, bindingRangeMask, ""});
}

void Factory::bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   const FactoryBindInfos & bindInfos)
{
    bind(targetComponentUUID, interfaceUUID, bindInfos);
    m_context->factoryMethods[bindInfos.componentUUID] = factoryFunc;
}

void Factory::bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   const uuids::uuid & instanceUUID, BindingScope scope,
                   uint8_t bindingRangeMask)
{
    bind(targetComponentUUID, interfaceUUID, factoryFunc, FactoryBindInfos{instanceUUID, scope, bindingRangeMask, ""});
}

void Factory::bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   const FactoryBindInfos & bindInfos)
{
    bind(targetComponentUUID, name, interfaceUUID, bindInfos);
    m_context->factoryMethods[bindInfos.componentUUID] = factoryFunc;
}

void Factory::bind(const uuids::uuid & targetComponentUUID, const std::string & name, const uuids::uuid & interfaceUUID,
                   const std::function<SRef<IComponentIntrospect>(void)> & factoryFunc,
                   const uuids::uuid & instanceUUID, BindingScope scope,
                   uint8_t bindingRangeMask)
{
    bind(targetComponentUUID, name, interfaceUUID, factoryFunc, FactoryBindInfos{instanceUUID, scope, bindingRangeMask, ""});
}


void Factory::bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                   const FactoryBindInfos & bindInfos)
{
    if (mapContains(m_context->specificBindings,targetComponentUUID)) {
        // bind already exists : error ???
    }
    m_context->specificBindings[targetComponentUUID][interfaceUUID] = bindInfos;
}


void Factory::bind(const uuids::uuid & targetComponentUUID, const uuids::uuid & interfaceUUID,
                   const uuids::uuid & instanceUUID, BindingScope scope,
                   uint8_t bindingRangeMask)
{
    bind(targetComponentUUID, interfaceUUID, FactoryBindInfos{instanceUUID, scope, bindingRangeMask, ""});
}

void Factory::bind(const uuids::uuid & targetComponentUUID, const std::string & name,
                   const uuids::uuid & interfaceUUID, const FactoryBindInfos & bindInfos)
{
    if (mapContains(m_context->specificNamedBindings,targetComponentUUID)) {
        // bind already exists : error ???
    }
    m_context->specificNamedBindings[targetComponentUUID][ make_pair(interfaceUUID,name)] = bindInfos;
}

void Factory::bind(const uuids::uuid & targetComponentUUID, const std::string & name,
                   const uuids::uuid & interfaceUUID, const uuids::uuid & instanceUUID,
                   BindingScope scope,
                   uint8_t bindingRangeMask)
{
    bind(targetComponentUUID, name, interfaceUUID, FactoryBindInfos{instanceUUID, scope, bindingRangeMask, ""});
}

FactoryBindInfos Factory::resolveBind(const uuids::uuid & interfaceUUID, std::deque<BindContext> contextLevels)
{
    // search for the closest specific declaration of binding for interfaceUUID : the first context is the last one, the latter is the first specific context created in the tree
    // WARNING : each bindingrange in the loop is replaced with the parent bindingrange : should it be the case ? may be not !
    for (auto [contextType,contextValue] : contextLevels) {
        if (contextType == ContextType::Component) {
            auto componentUUID = contextValue.componentUUID;
            if (mapContains(m_context->specificBindings, componentUUID)) {
                if (mapContains(m_context->specificBindings[componentUUID], interfaceUUID)) {
                    FactoryBindInfos bindInfos = m_context->specificBindings[componentUUID][interfaceUUID];
                    return bindInfos;
                }
            }
            if (contextValue.bindingRangeMask == BindingRange::Explicit) {
                // if the interface exists in core bindings, return the component
                if (mapContains (m_coreBindings, interfaceUUID)) {
                    return m_coreBindings.at(interfaceUUID);
                }
                // when current context binding range is explicit only, do not search forward when explicit bind isn't found
                throw InjectableNotFoundException("No explicit binding found to resolve component from interface UUID = " + uuids::to_string(interfaceUUID)
                                                  + " to inject to " +  uuids::to_string(componentUUID));
            }
            if (!(contextValue.bindingRangeMask & BindingRange::WithParents)) {
                break;
            }
        }
        //TODO : handle named contexts
    }
    // no specific binding found for this interface in contexts : search for a default binding
    if (contextLevels.empty() ||
            contextLevels.back().second.bindingRangeMask & (BindingRange::Default |BindingRange::All)) {
        if (mapContains(m_context->defaultBindings, interfaceUUID)) {
            FactoryBindInfos bindInfos = m_context->defaultBindings.at(interfaceUUID);
            return bindInfos;
        }
    }
    // no default binding found for this interface : search for an autobinding
    if (contextLevels.empty() ||
            contextLevels.back().second.bindingRangeMask & BindingRange::All) {
        if (mapContains(m_context->autoBindings,interfaceUUID)) {
            return m_context->autoBindings.at(interfaceUUID);
        }
    }
    // no binding found, is it a core binding ?
    if (mapContains (m_coreBindings, interfaceUUID)) {
        return m_coreBindings.at(interfaceUUID);
    }
    throw InjectableNotFoundException("No [auto|default|named] binding found to resolve component from interface UUID = " + uuids::to_string(interfaceUUID));
}

FactoryBindInfos Factory::resolveBind(const uuids::uuid & interfaceUUID, const std::string & name, std::deque<BindContext> contextLevels)
{
    pair<uuids::uuid, string> key = make_pair(interfaceUUID, name);

    // search for the closest specific declaration of binding for interfaceUUID : the first context is the last one, the latter is the first specific context created in the tree
    // WARNING : each bindingrange in the loop is replaced with the parent bindingrange : should it be the case ? may be not !
    for (auto [contextType,contextValue] : contextLevels) {
        if (contextType == ContextType::Component) {
            auto componentUUID = contextValue.componentUUID;
            if (mapContains(m_context->specificNamedBindings, componentUUID)) {
                if (mapContains(m_context->specificNamedBindings[componentUUID], key)) {
                    FactoryBindInfos bindInfos = m_context->specificNamedBindings[componentUUID][key];
                    return bindInfos;
                }
            }
            if (contextValue.bindingRangeMask == BindingRange::Explicit) {
                // when current context binding range is explicit only, do not search forward when explicit bind isn't found
                throw InjectableNotFoundException("No explicit binding found to resolve component from interface UUID = " + uuids::to_string(interfaceUUID) + " named " + name
                                                  + " to inject to " +  uuids::to_string(componentUUID));
            }
            if (!(contextValue.bindingRangeMask & BindingRange::WithParents)) {
                break;
            }
        }
        //TODO : handle named contexts
    }
    // no specific named binding found for this interface in contexts : search for a default named binding
    if (contextLevels.empty() ||
            contextLevels.back().second.bindingRangeMask & (BindingRange::Named |BindingRange::Default|BindingRange::All)) {
        if (mapContains(m_context->namedBindings,key)) {
            FactoryBindInfos bindInfos = m_context->namedBindings.at(key);
            return bindInfos;
        }
    }
    if (!contextLevels.empty() &&
            !(contextLevels.back().second.bindingRangeMask & (BindingRange::Default|BindingRange::All))) {
        throw InjectableNotFoundException("No default named binding found to resolve component from interface UUID = " + uuids::to_string(interfaceUUID) + " named " + name);
    }
    return resolveBind(interfaceUUID, contextLevels);
}

uuids::uuid Factory::getComponentUUID(const uuids::uuid & interfaceUUID)
{
    FactoryBindInfos bindInfos = resolveBind(interfaceUUID, {});
    return bindInfos.componentUUID;
}

uuids::uuid Factory::getComponentUUID(const uuids::uuid & interfaceUUID, const std::string & name)
{
    FactoryBindInfos bindInfos = resolveBind(interfaceUUID, name, {});
    return bindInfos.componentUUID;
}

void Factory::inject(SRef<IInjectable> component, std::deque<BindContext> contextLevels)
{
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Factory::inject";
#endif

    for (auto injectable : component->getInjectables()) {
        try {
            if (!injectable->isMulti()) {
                SRef<IComponentIntrospect> injectableRef = resolve(injectable, contextLevels);
                injectable->inject(injectableRef);
            }
            else {
                SRef<IEnumerable<SRef<IComponentIntrospect>>> injectableRef = resolveAll(injectable, contextLevels);
                injectable->inject(injectableRef);
            }
        }
        catch (const InjectableNotFoundException & e) {
            if (!injectable->optional()) { // only throw when injectable is mandatory
                throw e;
            }
        }
        catch (const Exception& e) {
            throw ConfigurationException(e.what());
        }
    }
    component->onInjected();
}

SRef<IComponentIntrospect> Factory::resolveComponent(const FactoryBindInfos & bindInfos, std::deque<BindContext> contextLevels)
{
    try 
    {
        uuids::uuid componentUUID = bindInfos.componentUUID;
        std::function<SRef<IComponentIntrospect>(void)> createComponent = [componentUUID, this]()-> SRef<IComponentIntrospect> {
            return resolveFromModule(componentUUID);
        };

        if (mapContains(m_context->factoryMethods, componentUUID)) {
            createComponent =  m_context->factoryMethods[componentUUID];
        }
        if (bindInfos.bindingRangeMask == BindingRange_Core) {
            createComponent =  m_coreFactoryMethods[componentUUID];
        }
    #ifdef XPCF_WITH_LOGS
        BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Factory::resolveComponent component uuid="<<uuids::to_string(componentUUID);
    #endif
        SRef<IComponentIntrospect> componentRef = createComponent();
        inject(componentRef->bindTo<IInjectable>(), contextLevels);
        if (componentRef->implements<IConfigurable>()) {
            fs::path configFilePath = m_propertyManager->getConfigPath(componentUUID);
            if (! configFilePath.empty()) {
                SRef<IConfigurable> iconf = componentRef->bindTo<IConfigurable>();
                XPCFErrorCode confErrCode;
                if (bindInfos.properties.empty()) {
                    confErrCode = iconf->configure(configFilePath.string().c_str());
                }
                else {
                    confErrCode = iconf->configure(configFilePath.string().c_str(), bindInfos.properties.c_str());
                }
                if (confErrCode != XPCFErrorCode::_SUCCESS) {
                    throw ConfigurationException("Configuration failed for component uuid=" + uuids::to_string(componentUUID), confErrCode);
                }
            }
        }
        return componentRef;
    }
    catch (const bcom::xpcf::Exception& e) {
        throw ConfigurationException(e.what());
    }
}

SRef<IComponentIntrospect> Factory::resolve(const uuids::uuid & interfaceUUID, std::deque<BindContext> contextLevels)
{
    try
    {
        FactoryBindInfos bindInfos = resolveBind(interfaceUUID, contextLevels);
    #ifdef XPCF_WITH_LOGS
        BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Factory::resolve interface uuid="<<uuids::to_string(interfaceUUID);
        BOOST_LOG_SEV(m_logger, logging::trivial::info)<<log(bindInfos).str();
    #endif
        contextLevels.push_front({ContextType::Component, bindInfos});
        if (bindInfos.scope == BindingScope::Singleton) {
            if (bindInfos.bindingRangeMask != BindingRange_Core) {
                if (! mapContains(m_singletonInstances, bindInfos.componentUUID)) {
                    SRef<IComponentIntrospect> componentRef = resolveComponent(bindInfos, contextLevels);
                    m_singletonInstances[bindInfos.componentUUID] = componentRef;
                }
                return m_singletonInstances.at(bindInfos.componentUUID);
            }
            else {
                if (! mapContains(m_coreInstances, bindInfos.componentUUID)) {
                    SRef<IComponentIntrospect> componentRef = resolveComponent(bindInfos, contextLevels);
                    m_coreInstances[bindInfos.componentUUID] = componentRef;
                }
                return m_coreInstances.at(bindInfos.componentUUID);
            }
        }
        SRef<IComponentIntrospect> componentRef = resolveComponent(bindInfos, contextLevels);
        return resolveComponent(bindInfos, contextLevels);
    }
    catch ( const bcom::xpcf::Exception& e) {
        throw ConfigurationException(e.what());
    }
}

SRef<IComponentIntrospect> Factory::resolve(const uuids::uuid & interfaceUUID, const string & name, std::deque<BindContext> contextLevels )
{
    FactoryBindInfos bindInfos = resolveBind(interfaceUUID, name, contextLevels);

    contextLevels.push_front({ContextType::Component, bindInfos});
    if (bindInfos.scope == BindingScope::Singleton) {
        pair<uuids::uuid,string> key = make_pair(bindInfos.componentUUID, name);
        if (! mapContains(m_namedSingletonInstances,key)) {
            SRef<IComponentIntrospect> componentRef = resolveComponent(bindInfos, contextLevels);
            m_namedSingletonInstances[key] = componentRef;
        }
        return m_namedSingletonInstances.at(key);
    }
    return resolveComponent(bindInfos, contextLevels);
}

SRef<IComponentIntrospect> Factory::resolve(const SRef<InjectableMetadata> & injectableInfo, std::deque<BindContext> contextLevels )
{
    if (injectableInfo->isNamed()) {
        return resolve(injectableInfo->getUuid(), injectableInfo->getName(), contextLevels);
    }
    return resolve(injectableInfo->getUuid(), contextLevels);
}

const SRef<IEnumerable<SRef<IComponentIntrospect>>> Factory::resolveAll(const SPtr<InjectableMetadata> & injectableInfo,
                                                                        std::deque<BindContext> contextLevels)
{
    return resolveAll(injectableInfo->getUuid(), contextLevels);
}

const SRef<IEnumerable<SRef<IComponentIntrospect>>> Factory::resolveAll(const uuids::uuid & interfaceUUID,
                                                                        std::deque<BindContext> contextLevels)
{
    SRef<ICollection<SRef<IComponentIntrospect>>> componentSet = utils::make_shared<Collection<SRef<IComponentIntrospect>,std::vector>>();
    if (!mapContains(m_context->multiBindings,interfaceUUID)) {
        //no explicit multibind : resolve any existing single bind default or explicit
        componentSet->add(resolve(interfaceUUID, contextLevels));
        return componentSet;
    }

    //multibind declared !
    for (auto bindInfos : m_context->multiBindings.at(interfaceUUID)) {
        contextLevels.push_front({ContextType::Component, bindInfos});
        if (bindInfos.scope == BindingScope::Singleton) {
            if (! mapContains(m_singletonInstances, bindInfos.componentUUID)) {
                SRef<IComponentIntrospect> componentRef = resolveComponent(bindInfos, contextLevels);
                m_singletonInstances[bindInfos.componentUUID] = componentRef;
            }
            componentSet->add(m_singletonInstances.at(bindInfos.componentUUID));
        }
        auto component = resolveComponent(bindInfos, contextLevels);
        componentSet->add(component);
    }
    return componentSet;
}

}}} //namespace org::bcom::xpcf
