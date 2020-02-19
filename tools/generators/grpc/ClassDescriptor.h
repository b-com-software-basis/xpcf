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

#ifndef CLASSDESCRIPTOR_H
#define CLASSDESCRIPTOR_H

#include <cppast/cpp_class.hpp>
#include "MethodDescriptor.h"
#include <map>
#include <vector>

class ClassDescriptor
{
public:
    ClassDescriptor(const cppast::cpp_entity& c);
    ClassDescriptor(const cppast::cpp_class& c);
    const std::string & getName() const { return m_baseClass.name(); }
    bool isInterface() { return m_isInterface; }
    bool parse(const cppast::cpp_entity_index& index);
    std::vector<MethodDescriptor> & methods() const { return  m_virtualMethods; }

private:
    void generateRpcMapping(const std::map<std::string, std::vector<std::size_t>> & virtualMethodsMap);
    mutable std::vector<MethodDescriptor> m_virtualMethods;
    //std::multimap<std::string, std::size_t> m_virtualMethodsMap;

    const cppast::cpp_class& m_baseClass;
    bool m_isInterface = false;

};

#endif // CLASSDESCRIPTOR_H
