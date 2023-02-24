/**
 * @copyright Copyright (c) 2019 B-com http://www.b-com.com/
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
 * @date 2019-06-13
 */


#include "private/xpcf/AliasManager.h"
#include <xpcf/api/IModuleManager.h>
#include <xpcf/core/ErrorMessage.h>
#include <xpcf/core/Exception.h>
#include <functional>

using namespace std;
using std::placeholders::_1;

#ifdef XPCF_WITH_LOGS
namespace logging = boost::log;
#endif

XPCF_DEFINE_FACTORY_CREATE_INSTANCE(org::bcom::xpcf::AliasManager);

namespace org { namespace bcom { namespace xpcf {
constexpr const char * XMLALIASNODE = "alias";

static const std::map<std::string,AliasManager::Type> aliasTypeConvertMap = {
    {"interface", AliasManager::Type::Interface},
    {"component", AliasManager::Type::Component},
    {"module", AliasManager::Type::Module}
};


void addAlias(std::map<std::string, uuids::uuid> & elementMap, const std::string & name, const uuids::uuid & uuid, bool forceReplace)
{
    if (!forceReplace) {
        if (elementMap.find(name) != elementMap.end()) {
            throw Exception("Alias already exists for name = " + name);
        }
    }
    elementMap[name] = uuid;
}

bool aliasExistsIn(std::map<std::string, uuids::uuid> & elementMap, const std::string & name)
{
    return (elementMap.find(name) != elementMap.end());
}

void AliasContext::clear()
{
    componentResolverMap.clear();
    interfaceResolverMap.clear();
    moduleResolverMap.clear();
}


AliasManager::AliasManager():ComponentBase(toUUID<AliasManager>())
{
    m_context = utils::make_shared<AliasContext>();
    declareInterface<IAliasManager>(this);
    declareInterface<AbstractAliasManager>(this);
    m_addAliasFunction[AliasManager::Type::Interface] = [&](const std::string & name, const uuids::uuid & uuid, bool forceReplace) {
        ::org::bcom::xpcf::addAlias(m_context->interfaceResolverMap, name, uuid, forceReplace);
    };

    m_addAliasFunction[AliasManager::Type::Component] = [&](const std::string & name, const uuids::uuid & uuid, bool forceReplace) {
        ::org::bcom::xpcf::addAlias(m_context->componentResolverMap, name, uuid, forceReplace);
    };

    m_addAliasFunction[AliasManager::Type::Module] = [&](const std::string & name, const uuids::uuid & uuid, bool forceReplace) {
        ::org::bcom::xpcf::addAlias(m_context->moduleResolverMap, name, uuid, forceReplace);
    };
#ifdef XPCF_WITH_LOGS
    m_logger.add_attribute("ClassName", boost::log::attributes::constant<std::string>("AliasManager"));
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"Constructor AliasManager::AliasManager () called!";
#endif
}

void AliasManager::clear()
{
    m_context->clear();
}

SRef<AliasContext> AliasManager::getContext() const
{
    return m_context;
}

void AliasManager::setContext(SRef<AliasContext> context)
{
    m_context = context;
}

void AliasManager::declareAlias(Type type, const std::string & name, const uuids::uuid & uuid)
{
    m_addAliasFunction[type](name,uuid,false);
}

void AliasManager::declareExplicitAlias(Type type, const std::string & name, const uuids::uuid & uuid)
{
    m_addAliasFunction[type](name,uuid,true);
}

bool AliasManager::aliasExists(Type type, const std::string & name)
{
    switch (type) {
    case IAliasManager::Type::Interface :
        return aliasExistsIn(m_context->interfaceResolverMap,name);
    case IAliasManager::Type::Component :
        return aliasExistsIn(m_context->componentResolverMap,name);
    case IAliasManager::Type::Module :
        return aliasExistsIn(m_context->moduleResolverMap,name);
    }
    // should never happen !!!
    return false;
}

void AliasManager::declareAliasNode(tinyxml2::XMLElement * xmlAliasElt)
{
    const char* aliasName = xmlAliasElt->Attribute("name");
    const char* aliasUuid = xmlAliasElt->Attribute("uuid");
    try {
        if (( aliasName == nullptr) || (xmlAliasElt->Attribute("type") == nullptr) || ( aliasUuid == nullptr)) {
            throw Exception("node incomplete : attributes name, type and uuid are mandatory.");
        }
        std::string typeStr = xmlAliasElt->Attribute("type");
        if (aliasTypeConvertMap.find(typeStr) == aliasTypeConvertMap.end()) {
            throw Exception("Invalid type attribute : type must be a value in {component|interface|module}. No other value is allowed.");
        }
        uuids::uuid uuid;
        try {
            uuid = toUUID( aliasUuid);
        }
        catch(const std::runtime_error &) {
            std::string what = "UUID format invalid in \"uuid\": ";
            what.append(aliasUuid);
            throw ConfigurationException(what);
        }

        AliasManager::Type type = aliasTypeConvertMap.at(typeStr);
        // replace any existing auto alias with explicit one from <aliases>!
        declareExplicitAlias(type, aliasName, uuid);
    }
    catch (const xpcf::Exception& e) {
        XmlErrorMessage msgErr("alias", e.what(), true);
        if(aliasUuid != nullptr) {
            msgErr.addAttribute("uuid", aliasUuid);
        }
        if(aliasName != nullptr) {
            msgErr.addAttribute("name", aliasName);
        }
        throw ConfigurationException(msgErr.write());
    }
}


void AliasManager::declareAliases(tinyxml2::XMLElement * xmlAliasManagerElt)
{
    try {
        processXmlNode(xmlAliasManagerElt, XMLALIASNODE, std::bind(&AliasManager::declareAliasNode, this, _1));
    }
    catch(const xpcf::Exception& e) {
        XmlErrorMessage msgErr("aliases", e.what(), true);
        throw ConfigurationException(msgErr.write());
    }
}

const uuids::uuid & AliasManager::resolveAlias(const std::string & name, const std::map<std::string, uuids::uuid> & elementMap)
{
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"AliasManager::resolveAlias name="<<name;
#endif
    if (elementMap.find(name) == elementMap.end()) {
#ifdef XPCF_WITH_LOGS
        BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"AliasManager::resolveAlias no alias found for "<<name;
#endif
        throw Exception("Unknown alias : no alias found for name " + name);
    }
#ifdef XPCF_WITH_LOGS
    BOOST_LOG_SEV(m_logger, logging::trivial::info)<<"AliasManager::resolveAlias name='"<<name<<"' resolved to uuid='"<<uuids::to_string(elementMap.at(name))<<"'";
#endif
    return elementMap.at(name);
}

const uuids::uuid & AliasManager::resolveComponentAlias(const std::string & name)
{
    return resolveAlias(name, m_context->componentResolverMap);
}

const uuids::uuid & AliasManager::resolveInterfaceAlias(const std::string & name)
{
    return resolveAlias(name, m_context->interfaceResolverMap);
}

const uuids::uuid & AliasManager::resolveModuleAlias(const std::string & name)
{
    return resolveAlias(name, m_context->moduleResolverMap);
}


}}} //namespace org::bcom::xpcf
