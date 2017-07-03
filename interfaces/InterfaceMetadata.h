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

#ifndef __InterfaceMetadata_H__
#define __InterfaceMetadata_H__

#include "XPCF_definitions.h"
#include <boost/uuid/uuid.hpp>
#include <string>

namespace org {
namespace bcom {
namespace xpcf {

class XPCF_EXPORT_API InterfaceMetadata {
public:
    InterfaceMetadata() = delete;
    InterfaceMetadata(const char * name, const uuids::uuid& interfaceUUID);
    InterfaceMetadata(const char * name, const char * interfaceUUID);
    virtual ~InterfaceMetadata() = default;

    const char * getDescription() const { return m_description.c_str(); }

    inline uuids::uuid getUUID() const { return m_uuid; }

private:
    void setUUID(const char * elementUUID);
    std::basic_string<char> m_description;
    uuids::uuid m_uuid;
};

}}} //namespace org::bcom::xpcf

#endif
