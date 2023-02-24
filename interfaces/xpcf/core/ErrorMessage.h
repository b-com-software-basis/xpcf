/**
 * @copyright Copyright (c) 2023 B-com http://www.b-com.com/
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
 * @author Adrien Schadle
 *
 * @file
 * @brief description of file
 * @date 2023-03-30
 */

#ifndef ORG_BCOM_XPCF_ERROR_MESSAGE_H
#define ORG_BCOM_XPCF_ERROR_MESSAGE_H

#include <string>
#include <vector>

namespace org { namespace bcom { namespace xpcf {

/**
 * @class XmlErrorMessage
 * @brief Specifies messages for xml error.
 */
class XmlErrorMessage {
public:
    XmlErrorMessage(std::string tag, std::string whatError, bool subElementError);

    ~XmlErrorMessage();

    // Add Xml attributes of the tag
    void addAttribute(std::string name, std::string value);

    // Write error message into a string
    std::string write();
private:
    std::string _tag;

    std::string _whatError;

    bool _subElementError;

    std::vector<std::pair<std::string, std::string>> _tagAttributes;
};

}}}


#endif
