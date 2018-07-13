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

#include "xpcf/api/ModuleMetadata.h"
#include "PathBuilder.h"
#include <string.h>

#include <boost/uuid/uuid_generators.hpp>
// implement Component
//


namespace org { namespace bcom { namespace xpcf {

using namespace uuids;

ModuleMetadata::ModuleMetadata(const char* name, const uuid& moduleID, const char *modulePath):InterfaceMetadata(name, moduleID)
{

    //TODO : null pointer exception ??? partial construction to avoid !!!
    setPath(modulePath);
}

ModuleMetadata::ModuleMetadata(const char *name, const char *moduleID, const char *modulePath):InterfaceMetadata(name, moduleID)
{
    setPath(modulePath);
}

ModuleMetadata::~ModuleMetadata()
{
}

void ModuleMetadata::setPath(const char* modulePath)
{
    if (modulePath != nullptr) {
        m_modulePath = modulePath;
    }
    m_moduleFullPath = PathBuilder::buildModuleFilePath(getDescription(), m_modulePath);
    if ( !boost::filesystem::exists( m_moduleFullPath ) )
    {
        //std::cout<<"Error : Module not found at : "<<m_moduleFullPath.c_str()<<std::endl;
    }
}

void ModuleMetadata::addComponent(const uuid& componentUUID)
{
    m_componentUUIDs.push_back(componentUUID);
}

uuid ModuleMetadata::getComponent(uint32_t index) const
{
    return m_componentUUIDs[index];
}

const char * ModuleMetadata::getComponentName(const uuids::uuid & componentUUID) const
{
    if (m_moduleComponentMap.find(componentUUID) != m_moduleComponentMap.end()) {
        return m_moduleComponentMap.at(componentUUID).c_str();
    }
    return nullptr;
}

void ModuleMetadata::declareComponent(const char * uuid, const char * name)
{//TODO : check
    uuids::uuid componentUUID = toUUID(uuid);
    m_componentUUIDs.push_back(componentUUID);
    m_moduleComponentMap[componentUUID] = name;
}

}}} //namespace org::bcom::xpcf
