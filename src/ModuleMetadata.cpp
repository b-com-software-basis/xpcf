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
#include "Collection.h"
#include <boost/uuid/uuid_generators.hpp>
// implement Component
//


namespace org { namespace bcom { namespace xpcf {

using namespace uuids;

class ModuleMetadata::ModuleMetadataImpl {
public:
    Collection<uuids::uuid,std::vector> m_componentUUIDs;
    Collection<SPtr<ComponentMetadata>,std::vector> m_components;
    std::map<uuids::uuid, SPtr<ComponentMetadata>> m_moduleComponentMap;
    std::basic_string<char> m_modulePath;
    fs::path m_moduleFullPath;
};

ModuleMetadata::ModuleMetadata(const char* name, const uuid& moduleID,
                               const char * description, const char *modulePath):
    InterfaceMetadata(name, moduleID, description),m_pimpl(new ModuleMetadataImpl())
{

    //TODO : null pointer exception ??? partial construction to avoid !!!
    setPath(modulePath);
}

ModuleMetadata::ModuleMetadata(const char *name, const char *moduleID,
                               const char * description, const char *modulePath):
    InterfaceMetadata(name, moduleID,description),m_pimpl(new ModuleMetadataImpl())
{
    setPath(modulePath);
}

ModuleMetadata::~ModuleMetadata()
{
}

void ModuleMetadata::setPath(const char* modulePath)
{
    if (modulePath != nullptr) {
        m_pimpl->m_modulePath = modulePath;
    }
    m_pimpl->m_moduleFullPath = PathBuilder::buildModuleFilePath(name(), m_pimpl->m_modulePath);
    if ( !boost::filesystem::exists( m_pimpl->m_moduleFullPath ) )
    {
        //std::cout<<"Error : Module not found at : "<<m_moduleFullPath.c_str()<<std::endl;
    }
}

void ModuleMetadata::addComponent(SPtr<ComponentMetadata> componentInfo)
{
    if (m_pimpl->m_moduleComponentMap.find(componentInfo->getUUID()) == m_pimpl->m_moduleComponentMap.end()) {
        m_pimpl->m_componentUUIDs.add(componentInfo->getUUID());
        m_pimpl->m_moduleComponentMap[componentInfo->getUUID()] = componentInfo;
        m_pimpl->m_components.add(componentInfo);
    }
}

void ModuleMetadata::removeComponent(const uuids::uuid & componentUUID)
{
    if (m_pimpl->m_moduleComponentMap.find(componentUUID) != m_pimpl->m_moduleComponentMap.end()) {
        m_pimpl->m_componentUUIDs.remove(componentUUID);
        SPtr<ComponentMetadata> componentInfo = m_pimpl->m_moduleComponentMap.at(componentUUID);
        m_pimpl->m_components.remove(componentInfo);
    }
}

const char * ModuleMetadata::getPath() const
{
    return m_pimpl->m_modulePath.c_str();
}

const fs::path & ModuleMetadata::getFullPath() const
{
    return m_pimpl->m_moduleFullPath;
}


const IEnumerable<uuids::uuid> & ModuleMetadata::getComponents() const
{
    return m_pimpl->m_componentUUIDs;
}

const IEnumerable<SPtr<ComponentMetadata>> & ModuleMetadata::getComponentsMetadata() const
{
    return m_pimpl->m_components;
}

SPtr<ComponentMetadata> ModuleMetadata::getComponentMetadata(const uuids::uuid & componentUUID) const
{
    if (m_pimpl->m_moduleComponentMap.find(componentUUID) != m_pimpl->m_moduleComponentMap.end()) {
        return m_pimpl->m_moduleComponentMap.at(componentUUID);
    }
    return nullptr;
}

void ModuleMetadata::declareComponent(const char * uuid, const char * name, const char * description)
{//TODO : check
    SPtr<ComponentMetadata> componentInfo = utils::make_shared<ComponentMetadata>(name, uuid, description);
    addComponent(componentInfo);
}

}}} //namespace org::bcom::xpcf
