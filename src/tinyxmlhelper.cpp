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

#include "private/xpcf/tinyxmlhelper.h"
#include <stdexcept>

std::string xmlGetTextSecure(tinyxml2::XMLElement * elt, const std::string & nodeName, bool optional, const std::string & defaultValue)
{
    if (elt->FirstChildElement(nodeName.c_str()) == nullptr) {
        throw std::runtime_error("Node '" + nodeName + "' doesn't exist in XML file");
    }
    if (elt->FirstChildElement(nodeName.c_str())->GetText() == nullptr) {
        if (optional) {
            return defaultValue;
        }
        throw std::runtime_error("Node '" + nodeName + "' doesn't contain a non empty value in XML file");
    }
    return elt->FirstChildElement(nodeName.c_str())->GetText();
}
