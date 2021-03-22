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
#include <xpcf/core/uuid.h>

class ClassDescriptor
{
public:
    typedef enum {
        INTERFACENAMESPACE,
        INTERFACEFILEPATH,
        REMOTINGNSPACE,
        GRPCSERVICENAME,
        GRPCPROTOFILENAME
    } MetadataType;
    ClassDescriptor(const cppast::cpp_entity& c, const std::string & nameSpace, const std::string & filePath);
    ClassDescriptor(const cppast::cpp_class& c);
    ClassDescriptor(const ClassDescriptor&  other) = delete;
    const std::string & getName() const { return m_baseClass.name(); }
    const org::bcom::xpcf::uuids::uuid & getClientUUID() const { return m_clientUUID; }
    const org::bcom::xpcf::uuids::uuid & createClientUUID() const;
    const org::bcom::xpcf::uuids::uuid & createServerUUID() const;
    const org::bcom::xpcf::uuids::uuid & getServerUUID() const { return m_serverUUID; }
    const std::vector<std::string> & getBases() { return m_bases; }
    bool isInterface() { return m_isInterface; }
    bool parse(const cppast::cpp_entity_index& index);
    bool ignored() const { return m_ignored; }
    const std::map<ClassDescriptor::MetadataType,std::string> & getMetadata() const { return m_metadata; }
    void setRemotingNamespace(const std::string & nspace) {
        m_metadata[MetadataType::REMOTINGNSPACE] = nspace;
    }

    std::string& operator[](const MetadataType& index)
    {
        return m_metadata[index];
    }

    //I is the index type, normally an int
    const std::string& operator[](const MetadataType& index) const
    {
        return m_metadata.at(index);
    }

    std::vector<SRef<MethodDescriptor>> & methods() const { return  m_virtualMethods; }

private:
    void generateRpcMapping(const std::map<std::string, std::vector<std::size_t>> & virtualMethodsMap);
    void parseMethods(const cppast::cpp_class & c, std::map<std::string, std::vector<std::size_t>> & virtualMethodsMap, const cppast::cpp_entity_index& index);
    mutable std::vector<SRef<MethodDescriptor>> m_virtualMethods;
    //std::multimap<std::string, std::size_t> m_virtualMethodsMap;
    bool m_ignored = false;
    const cppast::cpp_class& m_baseClass;
    bool m_isXpcfInterface = false;
    bool m_isInterface = false;
    bool m_isXpcfComponent = false;
    mutable org::bcom::xpcf::uuids::uuid m_clientUUID = {00000000-0000-0000-0000-000000000000};
    mutable org::bcom::xpcf::uuids::uuid m_serverUUID = {00000000-0000-0000-0000-000000000000};
    std::map<MetadataType,std::string> m_metadata;
    std::map<std::string,bool> m_classParsed;
    std::vector<std::string> m_bases;

};

#endif // CLASSDESCRIPTOR_H
