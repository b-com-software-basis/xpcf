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

#ifndef ORG_BCOM_XPCF_INTERFACEMETADATA_H
#define ORG_BCOM_XPCF_INTERFACEMETADATA_H

#include "xpcf/core/uuid.h"
#include "xpcf/xpcf_api_define.h"
#include "xpcf/core/refs.h"
#include "xpcf/core/traits.h"

namespace org { namespace bcom { namespace xpcf {

class XPCF_EXPORT_API InterfaceMetadata {
public:
    InterfaceMetadata() = delete;
    InterfaceMetadata(const InterfaceMetadata & other);
    InterfaceMetadata(InterfaceMetadata && other);
    InterfaceMetadata(const char * name, const uuids::uuid& interfaceUUID, const char * description);
    InterfaceMetadata(const char * name, const char * interfaceUUID, const char * description);
    virtual ~InterfaceMetadata();
    const char * name() const;
    const char * description() const;
    uuids::uuid getUUID() const;

    InterfaceMetadata & operator=(const InterfaceMetadata & other);
    InterfaceMetadata & operator=(InterfaceMetadata && other);
    bool operator==(const InterfaceMetadata & other);

private:
    class InterfaceMetadataImpl;
    UniqueRef<InterfaceMetadataImpl> m_pimpl;
};


template<typename M, typename C>
struct inferTrait
{
};

template<typename C>
struct inferTrait<InterfaceMetadata,C>
{
    typedef InterfaceTraits<C> InnerType;
};

template <typename M, typename C, typename T = typename inferTrait<M,C>::InnerType>
M to_metadata()
{
    static_assert(is_interface<C>::value || is_component<C>::value,
                  "Type passed to to_metadata is not an interface neither a component"
                  "or Traits not defined !!");
    return M( T::NAME, toUUID<T::UUID>(), T::DESCRIPTION);
}



}}} //namespace org::bcom::xpcf

#endif
