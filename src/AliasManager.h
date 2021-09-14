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


#include <xpcf/api/IAliasManager.h>
#include <xpcf/component/ComponentBase.h>
#include "tinyxmlhelper.h"

#ifdef XPCF_WITH_LOGS
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes.hpp>
#endif

#include <thread>
#include <vector>
#include <map>

namespace org { namespace bcom { namespace xpcf {

struct AliasContext {
    std::map<std::string, uuids::uuid> componentResolverMap;
    std::map<std::string, uuids::uuid> interfaceResolverMap;
    std::map<std::string, uuids::uuid> moduleResolverMap;
    void clear();
};

class AbstractAliasManager : virtual public IAliasManager {
public:
    virtual ~AbstractAliasManager() override = default;
    virtual void clear() = 0;
    virtual void declareAliases(tinyxml2::XMLElement * xmlModuleElt) = 0;
    virtual SRef<AliasContext> getContext() const = 0;
    virtual SRef<AliasContext> context() = 0;
    virtual void setContext(SRef<AliasContext> context) = 0;
};

template <> struct InterfaceTraits<AbstractAliasManager>
{
    static constexpr const char * UUID = "A05EB0B3-682E-4FAB-82B7-A3CBCAADE866";
    static constexpr const char * NAME = "XPCF::AbstractAliasManager";
    static constexpr const char * DESCRIPTION = "AbstractAliasManager interface.\nProvides named alias handling towards uuids (component, interface or module uuid) ";
};


class AliasManager : public ComponentBase,
        virtual public AbstractAliasManager {
public:
    AliasManager();
    ~AliasManager() override = default;
    void clear() override;
    void declareAliases(tinyxml2::XMLElement * xmlModuleElt) override;
    bool aliasExists(Type type, const std::string & name) override;
    void declareAlias(Type type, const std::string & name, const uuids::uuid & uuid) override;
    const uuids::uuid & resolveComponentAlias(const std::string & name) override;
    const uuids::uuid & resolveInterfaceAlias(const std::string & name) override;
    const uuids::uuid & resolveModuleAlias(const std::string & name) override;
    SRef<AliasContext> getContext() const override;
    SRef<AliasContext> context() override { return m_context; }
    void setContext(SRef<AliasContext> context) override;
    void unloadComponent () override final;

private:
    void declareExplicitAlias(Type type, const std::string & name, const uuids::uuid & uuid);
    void declareAliasNode(tinyxml2::XMLElement * xmlAliasElt);
    const uuids::uuid & resolveAlias(const std::string & name, const std::map<std::string, uuids::uuid> & elementMap);

    SRef<AliasContext> m_context;
    std::map<Type,std::function<void(const std::string & name, const uuids::uuid & uuid,bool)>> m_addAliasFunction;
#ifdef XPCF_WITH_LOGS
    boost::log::sources::severity_logger< boost::log::trivial::severity_level > m_logger;
#endif
};

template <> struct ComponentTraits<AliasManager>
{
    static constexpr const char * UUID = "007AF927-A585-4D2C-B604-9CF25B4B191A";
    static constexpr const char * NAME = "XPCF::AliasManager";
    static constexpr const char * DESCRIPTION = "XPCF::AliasManager";
};

}}} //namespace org::bcom::xpcf

#endif
