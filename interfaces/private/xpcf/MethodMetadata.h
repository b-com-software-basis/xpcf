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

#ifndef ORG_BCOM_XPCF_METHODMETADATA_H
#define ORG_BCOM_XPCF_METHODMETADATA_H


#include "xpcf/xpcf_api_define.h"
#include "xpcf/core/uuid.h"
#include "xpcf/core/refs.h"
#include "xpcf/api/InterfaceMetadata.h"
#include "xpcf/collection/IEnumerable.h"

#include <string>
#include <vector>

namespace org { namespace bcom { namespace xpcf {
/**
 * @class MethodMetadata
 * @brief Specifies the MethodMetadata class.
 *
 * This class provides XPCF method metadata informations.
 */
class XPCF_EXPORT_API MethodMetadata : public InterfaceMetadata {
public:
    MethodMetadata() = delete;
    MethodMetadata(const MethodMetadata & other);
    MethodMetadata(MethodMetadata && other);
    virtual ~MethodMetadata();


    MethodMetadata& operator=(const MethodMetadata & other);
    MethodMetadata& operator=(MethodMetadata && other);
    bool operator==(const MethodMetadata & other);

};

}}} //namespace org::bcom::xpcf

#endif
