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

#ifndef ORG_BCOM_XPCF_COMPONENTMETADATA_H
#define ORG_BCOM_XPCF_COMPONENTMETADATA_H


#include "xpcf/xpcf_api_define.h"
#include "xpcf/core/uuid.h"
#include "xpcf/core/refs.h"
#include "xpcf/api/InterfaceMetadata.h"
#include "xpcf/collection/IEnumerable.h"

#include <string>
#include <vector>

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API ComponentMetadata : public InterfaceMetadata {
public:
    ComponentMetadata() = delete;
    ComponentMetadata(const ComponentMetadata & other);
    ComponentMetadata(ComponentMetadata && other);
    ComponentMetadata(const char* name, const uuids::uuid& componentID, const char* description);
    ComponentMetadata(const char* name, const char *componentID, const char* description);
    virtual ~ComponentMetadata();

    void addInterface(const uuids::uuid& interfaceUUID);
    const IEnumerable<uuids::uuid> & getInterfaces() const;

    ComponentMetadata& operator=(const ComponentMetadata & other);
    ComponentMetadata& operator=(ComponentMetadata && other);
    bool operator==(const ComponentMetadata & other);

private:
    class ComponentMetadataImpl;
    UniqueRef<ComponentMetadataImpl> m_pimpl;

};


//template<typename C>
//struct inferTrait<ComponentMetadata, C>
//{
//    typedef ComponentTraits<C> InnerType;
//};

}}} //namespace org::bcom::xpcf

#endif
