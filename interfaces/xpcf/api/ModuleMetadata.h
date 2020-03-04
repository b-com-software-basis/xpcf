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

#ifndef ORG_BCOM_XPCF_MODULEMETADATA_H
#define ORG_BCOM_XPCF_MODULEMETADATA_H

#include "xpcf/module/IModuleIndex.h"
#include "xpcf/api/InterfaceMetadata.h"

#include <string>
#include <vector>
#include <map>

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API ModuleMetadata : public InterfaceMetadata, public IModuleIndex
{
public:
    ModuleMetadata() = delete;
    ModuleMetadata(const char* name, const uuids::uuid& moduleID, const char * description, const char *modulePath);
    ModuleMetadata(const char* name, const char *moduleID, const char * description, const char *modulePath);
    virtual ~ModuleMetadata() override;

    void addComponent(SPtr<ComponentMetadata> componentInfo);
    void removeComponent(const uuids::uuid & componentUUID);
    const char * getPath() const;
    const char * getFullPath() const;

    const IEnumerable<uuids::uuid> & getComponents() const override;
    const IEnumerable<SPtr<ComponentMetadata>> & getComponentsMetadata() const override;
    SPtr<ComponentMetadata> getComponentMetadata(const uuids::uuid & componentUUID) const override;
    void declareComponent(const char * uuid, const char * name, const char * description) override;

private:
    void setPath(const char* modulePath);
    class ModuleMetadataImpl;
    UniqueRef<ModuleMetadataImpl> m_pimpl;
};

}}} //namespace org::bcom::xpcf

#endif
