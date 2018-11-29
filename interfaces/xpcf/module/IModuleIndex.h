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

#ifndef ORG_BCOM_XPCF_IMODULEINDEX_H
#define ORG_BCOM_XPCF_IMODULEINDEX_H

#include "xpcf/core/uuid.h"
#include "xpcf/api/ComponentMetadata.h"
#include "xpcf/collection/IEnumerable.h"

namespace org { namespace bcom { namespace xpcf {

class IModuleIndex {
public:
    virtual ~IModuleIndex() = default;
    virtual const IEnumerable<uuids::uuid> & getComponents() const = 0;
    virtual const IEnumerable<SPtr<ComponentMetadata>> & getComponentsMetadata() const = 0;
    virtual SPtr<ComponentMetadata> getComponentMetadata(const uuids::uuid & componentUUID) const = 0;
    virtual void declareComponent(const char * uuid, const char * name, const char * description) = 0;
    template <typename T> void declareComponent();
};
//todo: add ComponentMetadata from traits and provide getComponentMetadata method
// Remove moduleuuid from CompMdata and configpath
// study moduleuuid binding with componentuuid outside of compmdata ??
// study method introspection with boost::XXX to generate declarative method description : clang frontend ??

template <typename T>
inline void IModuleIndex::declareComponent()
{
     static_assert(is_component<T>::value,
                   "Type passed to declareComponent is not a component "
                   "or componentTraits not defined !!");
     declareComponent(ComponentTraits<T>::UUID, ComponentTraits<T>::NAME, ComponentTraits<T>::DESCRIPTION);
}

}}}

#endif
