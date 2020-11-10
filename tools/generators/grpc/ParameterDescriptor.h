/**
 * @copyright Copyright (c) 2019 B-com http://www.b-com.com/
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
 * @date 2019-11-15
 */

#ifndef PARAMETERDESCRIPTOR_H
#define PARAMETERDESCRIPTOR_H
#include <cppast/cpp_function.hpp>
#include <string>
#include "TypeDescriptor.h"



class ParameterDescriptor
{
public:
    typedef enum  {
        none = 0x0,
        in = 0x01,
        out = 0x02,
        inout = 0x03
    } io_type;

    ParameterDescriptor(const cppast::cpp_function_parameter & p);
    //void addAttribute();
    const io_type & ioType() const { return m_ioType; }

    const cppast::cpp_type & getCppastType() const { return m_typeDescriptor.getCppastType(); }
    const std::string & getName() const { return m_name; }
    void setName(const std::string & name) { m_name = name; }
    const std::string & getNamespace() const { return m_typeDescriptor.getNamespace(); }
    void setNamespace(const std::string & nameSpace) { m_typeDescriptor.setNamespace(nameSpace); }



    const TypeDescriptor & type() const { return m_typeDescriptor; }
    bool parse(const cppast::cpp_entity_index& index);
    //std::string getInnerType();

private:
    void setIOType(const std::string & ioType);
    void parse_parameter(const cppast::cpp_entity_index& index, const cppast::cpp_type & p);
    const cppast::cpp_function_parameter & m_baseParam;
    io_type m_ioType = io_type::none;
    ParameterDescriptor * m_child;
    TypeDescriptor m_typeDescriptor;
    std::string m_name;
};

#endif // PARAMETERDESCRIPTOR_H
