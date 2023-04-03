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

#include <iostream>

#include <xpcf/core/ErrorMessage.h>

namespace org { namespace bcom { namespace xpcf {

XmlErrorMessage::XmlErrorMessage(std::string tag, std::string what, bool subElementError):
    _tag(tag)
    , _whatError(what)
    , _subElementError(subElementError)
{
}

XmlErrorMessage::~XmlErrorMessage() 
{

}

void XmlErrorMessage::addAttribute(std::string name, std::string value)
{
    _tagAttributes.push_back(make_pair(name, value));
}

std::string XmlErrorMessage::write()
{
    std::string errorMessage = "<"; 
    errorMessage.append(_tag);
    std::vector<std::pair<std::string, std::string>>::iterator iter = _tagAttributes.begin();
    while(iter!=_tagAttributes.end()) {
        errorMessage.append(" ");
        errorMessage.append(iter->first);
        errorMessage.append("=\"");
        errorMessage.append(iter->second);
        errorMessage.append("\"");
        iter++;
    }
    if(!_subElementError) {
        errorMessage.append(" error=\"");
        errorMessage.append(_whatError);
        errorMessage.append("\">");
    }
    else {
        errorMessage.append(">");
        errorMessage.append(_whatError);
    }

    return errorMessage;
}

}}}