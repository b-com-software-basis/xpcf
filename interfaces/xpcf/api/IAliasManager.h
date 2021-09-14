/**
 * @copyright Copyright (c) 2021 B-com http://www.b-com.com/
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
 * @date 2021-09-14
 */

#ifndef IALIASMANAGER_H
#define IALIASMANAGER_H

#include <xpcf/api/IComponentIntrospect.h>

namespace org { namespace bcom { namespace xpcf {

class IAliasManager : virtual public IComponentIntrospect {
public:
    typedef enum {
        Interface,
        Component,
        Module
    } Type;

    virtual ~IAliasManager() override = default;
    virtual bool aliasExists(Type type, const std::string & name) = 0;
    virtual void declareAlias(Type type, const std::string & name, const uuids::uuid & uuid) = 0;
    virtual const uuids::uuid & resolveComponentAlias(const std::string & name) = 0;
    virtual const uuids::uuid & resolveInterfaceAlias(const std::string & name) = 0;
    virtual const uuids::uuid & resolveModuleAlias(const std::string & name) = 0;

    template <typename I> void declareInterfaceAlias(const std::string & name);
    template <typename C> void declareComponentAlias(const std::string & name);
};

template <typename I> void IAliasManager::declareInterfaceAlias(const std::string & name)
{
    declareAlias(IAliasManager::Type::Interface, name, toUUID<I>());
}

template <typename C> void IAliasManager::declareComponentAlias(const std::string & name)
{
    declareAlias(IAliasManager::Type::Component, name, toUUID<C>());
}

template <> struct InterfaceTraits<IAliasManager>
{
    static constexpr const char * UUID = "2855F879-52AA-42F6-A84A-D9D3FD37D28F";
    static constexpr const char * NAME = "XPCF::IAliasManager";
    static constexpr const char * DESCRIPTION = "Alias manager interface.\nProvides named alias handling towards uuids (component, interface or module uuid) ";
};

}}}


#endif // IALIASMANAGER_H
