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

#ifndef ORG_BCOM_XPCF_ALIASMANAGER_H
#define ORG_BCOM_XPCF_ALIASMANAGER_H

//#define BOOST_ALL_DYN_LINK 1


#include <xpcf/api/IComponentManager.h>
#include <xpcf/component/ComponentBase.h>
#include "tinyxmlhelper.h"

#include <thread>
#include <vector>
#include <map>

namespace org { namespace bcom { namespace xpcf {

class IAliasManager : virtual public IComponentIntrospect {
public:
    typedef enum {
        Interface,
        Component,
        Module
    } Type;

    virtual ~IAliasManager() override = default;
    virtual void clear() = 0;
    virtual void declareAliases(tinyxml2::XMLElement * xmlModuleElt) = 0;
    virtual bool aliasExists(Type type, const std::string & name) = 0;
    virtual void declareAlias(Type type, const std::string & name, const uuids::uuid & uuid) = 0;
    virtual const uuids::uuid & resolveComponentAlias(const std::string & name) const = 0;
    virtual const uuids::uuid & resolveInterfaceAlias(const std::string & name) const = 0;
    virtual const uuids::uuid & resolveModuleAlias(const std::string & name)const = 0;
};

template <> struct InterfaceTraits<IAliasManager>
{
    static constexpr const char * UUID = "2855F879-52AA-42F6-A84A-D9D3FD37D28F";
    static constexpr const char * NAME = "XPCF::IAliasManager";
    static constexpr const char * DESCRIPTION = "Alias manager interface.\nProvides named alias handling towards uuids (component, interface or module uuid) ";
};


class AliasManager : public ComponentBase,
        virtual public IAliasManager {
public:
    AliasManager();
    ~AliasManager() override = default;
    void clear() override;
    void declareAliases(tinyxml2::XMLElement * xmlModuleElt) override;
    bool aliasExists(Type type, const std::string & name) override;
    void declareAlias(Type type, const std::string & name, const uuids::uuid & uuid) override;
    const uuids::uuid & resolveComponentAlias(const std::string & name) const override;
    const uuids::uuid & resolveInterfaceAlias(const std::string & name) const override;
    const uuids::uuid & resolveModuleAlias(const std::string & name)const override;
    void unloadComponent () override final;

private:
    void declareExplicitAlias(Type type, const std::string & name, const uuids::uuid & uuid);
    void declareAliasNode(tinyxml2::XMLElement * xmlAliasElt);
    std::map<std::string, uuids::uuid> m_componentResolverMap;
    std::map<std::string, uuids::uuid> m_interfaceResolverMap;
    std::map<std::string, uuids::uuid> m_moduleResolverMap;
    std::map<Type,std::function<void(const std::string & name, const uuids::uuid & uuid,bool)>> m_addAliasFunction;
};

template <> struct ComponentTraits<AliasManager>
{
    static constexpr const char * UUID = "007AF927-A585-4D2C-B604-9CF25B4B191A";
    static constexpr const char * NAME = "XPCF::AliasManager";
    static constexpr const char * DESCRIPTION = "XPCF::AliasManager";
};

}}} //namespace org::bcom::xpcf

#endif
