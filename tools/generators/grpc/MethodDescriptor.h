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

#ifndef METHODDESCRIPTOR_H
#define METHODDESCRIPTOR_H

#include <vector>
#include <string>
#include <cppast/cpp_member_function.hpp>
#include <ParameterDescriptor.h>

// note : deducing a method parameter is [in] means :
// - plain T with no ref or pointer or shared_ptr
// - for pointers : one const on each derefed level i.e. const T* const T * const * const * and so on.
// - for refs :  one const on each derefed level i.e. const T & (== T const &), T const * const & .. and so on.
// - for shared_ptr : const shared_ptr<T>
// deducing a method parameter is [out] means :
// - T & with no const
// - T(*)n with no const
// mixed or inout types are not deduced and must be disambiguated with custom attributes at the parameter level : [[xpcf::in]], [[xpcf::out]], [[xpcf::inout]]
// streaming rpc must be indicated with [[grpc::server_streaming]], [[grpc::client_streaming]] or [[grpc::streaming]] at the method level
// optionally, grpc request and response message name can be set with [[grpc::request("requestMessageName")]] and [[grpc::response("responseMessageName")]]
// However, if you set the request and response across several methods, and use the same messages for several methods, you are responsible to ensure that message content are consistent
// Otherwise, request message content is built from input and input/output parameters
// response message content is built from output and input/output parameters

class MethodDescriptor
{
public:
    typedef enum {
        none = 0x0,
        client = 0x01,
        server = 0x02,
        bidir = 0x03
    } streaming_type;

    MethodDescriptor(const cppast::cpp_entity& m);
    MethodDescriptor(const cppast::cpp_member_function& m);
    const std::string & getName() const { return m_baseMethod.name(); }
    const TypeDescriptor & returnType() const { return m_returnDescriptor; }
    const std::string getFullDeclaration() const { return m_returnType + " " + m_declaration; }
    const std::string & getDeclaration() const { return m_declaration; }
    const std::string & getReturnType() const { return m_returnType; }
    const streaming_type & streamingType() const { return m_rpcStreamingType; }
  /*  void addAttribute();
    bool isClientStream();
    bool isServerStream();
    void addParameter();*/
    bool isPureVirtual() const { return m_pureVirtual; }
    bool parse(const cppast::cpp_entity_index& index);
    bool hasInputs() { return ((m_inParams.size() != 0) || (m_inoutParams.size() != 0)); }
    bool hasOutputs() { return ((m_inoutParams.size() != 0) || (m_outParams.size() != 0 ) || !m_returnDescriptor.isVoid() ); }

    std::vector<ParameterDescriptor> m_inParams;
    std::vector<ParameterDescriptor> m_outParams;
    std::vector<ParameterDescriptor> m_inoutParams;
    std::string m_requestName = "Empty";
    std::string m_responseName = "Empty";
    std::string m_rpcName;

private:
    void setStreamingType(const std::string & streamType);
    TypeDescriptor m_returnDescriptor;
    streaming_type m_rpcStreamingType = streaming_type::none;
    const cppast::cpp_member_function& m_baseMethod;
    bool m_pureVirtual = false;
    std::string m_declaration;
    std::string m_returnType;
};

#endif // METHODDESCRIPTOR_H
