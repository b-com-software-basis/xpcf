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

#ifndef ORG_BCOM_XPCF_TINYXMLHELPER_H
#define ORG_BCOM_XPCF_TINYXMLHELPER_H

#include <string>
#include "tinyxml2.h"
#include <functional>
#include <xpcf/core/XPCFErrorCode.h>
#include <xpcf/core/refs.h>

std::string xmlGetTextSecure(tinyxml2::XMLElement * elt, const std::string & nodeName,
                             bool optional = false, const std::string & defaultValue = "");

namespace org { namespace bcom { namespace xpcf {

inline void processXmlNode(tinyxml2::XMLElement * xmlElt, const char * nodeName, std::function<void(tinyxml2::XMLElement*)>  func)
{
    tinyxml2::XMLElement *element = xmlElt->FirstChildElement(nodeName);
    while (element != nullptr) {
        func(element);
        element = element->NextSiblingElement(nodeName);
    }
}
/*
template <typename T>
void processXmlNode(SRef<T> element, tinyxml2::XMLElement * xmlParentElt, const char * nodeName, std::function<XPCFErrorCode(SRef<T>, tinyxml2::XMLElement*)>  func)
{
    tinyxml2::XMLElement *xmlElt = xmlParentElt->FirstChildElement(nodeName);
    while (xmlElt != nullptr) {
        func(element, xmlElt);
        element = element->NextSiblingElement(nodeName);
    }
}*/

}}}
#endif // TINYXMLHELPER_H
