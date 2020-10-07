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

#ifndef TYPEDESCRIPTOR_H
#define TYPEDESCRIPTOR_H
#include <cppast/cpp_entity_index.hpp>
#include <cppast/cpp_type.hpp>
#include <string>
#include <deque>
#include <map>
#include <iostream>

// template typename T to_proto/from_proto OR another solution with IGrpcProtoInterface / IRemotingBufferInterface ??
// Simple backend [proto|flat] buffers
// Data class C either :
// - inherits from [proto|flat] buffer message class. C.[proto|fbs] file exists
// - can be converted to [proto|flat]::C class with template methods : a C.proto or C.fbs file must exists in the source folder
// - inherits ISerializable ([de]serialize from/to bytes buffer)
// - can be serialized to bytes buffer with template overload

//XPCF remoting study
//Message options:
//1/ custom buffered messages
//- serialisation interface
//- serialisation  generation from cppast using flexbuffers, capnproto. ... or dedicated serialisation type

//2/ standard messages based upon proto/flat buffers
//- proto/flat buffers file generation

//Service :
//1 -> type agnostic in/out service for serialisation generation/interface
//2 -> proto/flat based service for proto/flat defined datastructure

//serialisation generation/interface :
//-> pros :
//- allows an easier integration in existing c++ code

//-> cons :
//- needs to generate deserialisation code in other languages too

//proto/flat buffers messages :
//-> pros :
//- cross languages deserialisation is provided out of the box

//-> cons :
//- needs to generate/create proto files
//- adds a dependency toward proto/flat buffers plugin for grpc

//Endpoint notion ? Interface with destination configuration ?

enum cpp_builtin_type {
    cpp_void, //< `void`

    cpp_bool, //< `bool`

    cpp_uchar,     //< `unsigned char`
    cpp_ushort,    //< `unsigned short`
    cpp_uint,      //< `unsigned int`
    cpp_ulong,     //< `unsigned long`
    cpp_ulonglong, //< `unsigned long long`
    cpp_uint128,   //< `unsigned __int128`

    cpp_schar,    //< `signed char`
    cpp_short,    //< `short`
    cpp_int,      //< `int`
    cpp_long,     //< `long`
    cpp_longlong, //< `long long`
    cpp_int128,   //< `__int128`

    cpp_float,      //< `float`
    cpp_double,     //< `double`
    cpp_longdouble, //< `long double`
    cpp_float128,   //< `__float128`

    cpp_char,   //< `char`
    cpp_wchar,  //< `wchar_t`
    cpp_char16, //< `char16_t`
    cpp_char32, //< `char32_t`

    cpp_nullptr, //< `decltype(nullptr)` aka `std::nullptr_t`
    cpp_int8_t,
    cpp_int16_t,
    cpp_int32_t,
    cpp_int64_t,

    cpp_uint8_t,
    cpp_uint16_t,
    cpp_uint32_t,
    cpp_uint64_t,

    cpp_int_least8_t,
    cpp_int_least16_t,
    cpp_int_least32_t,
    cpp_int_least64_t,

    cpp_uint_least8_t,
    cpp_uint_least16_t,
    cpp_uint_least32_t,
    cpp_uint_least64_t,

    cpp_int_fast8_t,
    cpp_int_fast16_t,
    cpp_int_fast32_t,
    cpp_int_fast64_t,

    cpp_uint_fast8_t,
    cpp_uint_fast16_t,
    cpp_uint_fast32_t,
    cpp_uint_fast64_t,

    cpp_intptr_t,
    cpp_uintptr_t,

    cpp_intmax_t,
    cpp_uintmax_t
};

enum type_kind {
    builtin_t,
    std_string_t,
    user_defined_t,
    array_t,
    enum_t,
    template_t
};

enum template_type {
    none,
    vector_t,
    map_t,
    tuple_t,
    sharedptr_t
};

class TypeDescriptor
{
public:
    struct TypeDescriptorInfo {
        TypeDescriptorInfo() = default;
        TypeDescriptorInfo(const std::string & typeName, template_type tmplType):m_typename(typeName),m_templateType(tmplType) {}
        std::string m_typename;
        bool m_const = true;
        bool m_isBuiltin;
        bool m_isEnum;
        bool m_sharedRef = false;
        bool m_isReference = false;
        bool m_isContainer = false;
        bool m_void = true;
        bool isLeaf = false; // false for intermediate template
        type_kind m_kind;
        template_type m_templateType;
        cpp_builtin_type m_builtinType;
        std::vector<std::shared_ptr<TypeDescriptor>> m_tmplArgsVector; // only for template type desc
    };
    TypeDescriptor();
    TypeDescriptor(const TypeDescriptorInfo & info);
    TypeDescriptor(const std::string & typeName, template_type tmplType);
    void parse(const cppast::cpp_entity_index& index, const cppast::cpp_type & p);
    bool isConst() const { return m_descriptorInfo.m_const; }
    bool isSharedRef() const { return m_descriptorInfo.m_sharedRef; }
    bool isReference() const { return m_descriptorInfo.m_isReference; }
    bool isVoid() const { return m_descriptorInfo.m_void; }
    void addTemplateArgument(const std::string & arg, const std::shared_ptr<TypeDescriptor> & d);
    enum type_kind kind() const { return m_descriptorInfo.m_kind; }
    const std::string & getTypename() const { return m_descriptorInfo.m_typename; }
    enum template_type getTemplateType() const { return m_descriptorInfo.m_templateType; }
    enum cpp_builtin_type getBuiltinType() const { return m_descriptorInfo.m_builtinType; }
    std::string getFullTypeDescription() const;

private:
    TypeDescriptor::TypeDescriptorInfo parseType(const cppast::cpp_entity_index& index, const cppast::cpp_type & p);
    std::string parseTemplateArguments(const std::string & argStr, uint32_t groupNumber = 0);
    TypeDescriptor::TypeDescriptorInfo parseTemplateInstanciation(const cppast::cpp_entity_index& index, const cppast::cpp_type & p);
    void linkArgumentsType(const std::string & args, const std::map<std::string,std::pair<std::string,std::shared_ptr<TypeDescriptor>>> & templateGroups);

    std::shared_ptr<TypeDescriptor> deduceTemplateType(const std::string & leafTemplate);
    std::map<std::string,std::pair<std::string,std::shared_ptr<TypeDescriptor>>> m_templateGroups;
    std::map<std::string,std::shared_ptr<TypeDescriptor>> m_tmplArgs; // only for template type desc
    std::string m_typename;
    //bool isLeaf = false; // false for intermediate template
    std::deque<bool> m_foundConst;
    std::deque<bool> m_foundRef;
    TypeDescriptorInfo m_descriptorInfo;
};

inline void waitForUserInput()
{
    int i;
    std::cin >> i;
}

#endif // TYPEDESCRIPTOR_H
