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


#include "AliasManager.h"
#include <xpcf/api/IModuleManager.h>
#include <xpcf/core/Exception.h>
#include <functional>

using namespace std;
using std::placeholders::_1;
namespace fs = boost::filesystem;

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

AliasManager::AliasManager():ComponentBase(toUUID<AliasManager>())
{
    declareInterface<IAliasManager>(this);
    m_addAliasFunction[AliasManager::Type::Interface] = [&](const std::string & name, const uuids::uuid & uuid, bool forceReplace) {
        ::org::bcom::xpcf::addAlias(m_interfaceResolverMap, name, uuid, forceReplace);
    };

    m_addAliasFunction[AliasManager::Type::Component] = [&](const std::string & name, const uuids::uuid & uuid, bool forceReplace) {
        ::org::bcom::xpcf::addAlias(m_componentResolverMap, name, uuid, forceReplace);
    };

    m_addAliasFunction[AliasManager::Type::Module] = [&](const std::string & name, const uuids::uuid & uuid, bool forceReplace) {
        ::org::bcom::xpcf::addAlias(m_moduleResolverMap, name, uuid, forceReplace);
    };
}

void AliasManager::clear()
{
    m_moduleResolverMap.clear();
    m_interfaceResolverMap.clear();
    m_componentResolverMap.clear();
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
    switch(type) {
        case IAliasManager::Type::Interface :
            return aliasExistsIn(m_interfaceResolverMap,name);
        case IAliasManager::Type::Component :
            return aliasExistsIn(m_componentResolverMap,name);
        case IAliasManager::Type::Module :
            return aliasExistsIn(m_moduleResolverMap,name);
    }
    // should never happen !!!
    return false;
}

void AliasManager::declareAliasNode(tinyxml2::XMLElement * xmlAliasElt)
{
    if ((xmlAliasElt->Attribute("name") == nullptr) || (xmlAliasElt->Attribute("type") == nullptr) || (xmlAliasElt->Attribute("uuid") == nullptr)) {
        throw Exception("<alias name=\"...\" type=\"[component|interface|module]\" uuid=\"...\"/> node incomplete : attributes name, type and uuid are mandatory.");
    }
    std::string typeStr = xmlAliasElt->Attribute("type");
    if (aliasTypeConvertMap.find(typeStr) == aliasTypeConvertMap.end()) {
        throw Exception("Invalid type attribute : type must be a value in {component|interface|module}. No other value is allowed.");
    }
    uuids::uuid uuid =  toUUID( xmlAliasElt->Attribute("uuid"));
    AliasManager::Type type = aliasTypeConvertMap.at(typeStr);
    std::string name = xmlAliasElt->Attribute("name");
    // replace any existing auto alias with explicit one from <aliases>!
    declareExplicitAlias(type, name, uuid);
}


void AliasManager::declareAliases(tinyxml2::XMLElement * xmlAliasManagerElt)
{
    processXmlNode(xmlAliasManagerElt, XMLALIASNODE, std::bind(&AliasManager::declareAliasNode, this, _1));
}

const uuids::uuid & resolveAlias(const std::string & name, const std::map<std::string, uuids::uuid> & elementMap)
{
    if (elementMap.find(name) == elementMap.end()) {
        throw Exception("Unknown alias : no alias found for name " + name);
    }
    return elementMap.at(name);
}

const uuids::uuid & AliasManager::resolveComponentAlias(const std::string & name) const
{
    return ::org::bcom::xpcf::resolveAlias(name, m_componentResolverMap);
}

const uuids::uuid & AliasManager::resolveInterfaceAlias(const std::string & name) const
{
    return ::org::bcom::xpcf::resolveAlias(name, m_interfaceResolverMap);
}

const uuids::uuid & AliasManager::resolveModuleAlias(const std::string & name) const
{
    return ::org::bcom::xpcf::resolveAlias(name, m_moduleResolverMap);
}


}}} //namespace org::bcom::xpcf
