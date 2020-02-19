#include "ParameterDescriptor.h"

#include <cppast/cpp_member_function.hpp>
#include <cppast/cpp_variable.hpp>
#include <cppast/cpp_template.hpp>
#include <cppast/cpp_array_type.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_entity.hpp>
#include <iostream>
#include <map>


ParameterDescriptor::ParameterDescriptor(const cppast::cpp_function_parameter & p):baseParam(p)
{

}

const std::map<std::string,ParameterDescriptor::io_type> str2typeIoMap =
{
    {"inout",ParameterDescriptor::io_type::inout},
    {"in",ParameterDescriptor::io_type::in},
    {"out",ParameterDescriptor::io_type::out},
};

void ParameterDescriptor::setIOType(const std::string & ioType)
{
    if (str2typeIoMap.find(ioType) != str2typeIoMap.end()) {
        if (((m_ioType == io_type::in) && (str2typeIoMap.at(ioType) == io_type::out)) ||
            ((m_ioType == io_type::out) && (str2typeIoMap.at(ioType) == io_type::in))) {
            m_ioType = io_type::inout;
        }
        else {
            m_ioType = str2typeIoMap.at(ioType);
        }
    }
}

bool ParameterDescriptor::parse(const cppast::cpp_entity_index& index)
{
    std::cout << " ==> parsing parameter "<<baseParam.name()<<'\n';
    m_typeDescriptor.parse(index, baseParam.type());
    if (!baseParam.attributes().empty()) {
        // handle parameter attributes
        for (auto & attrib : baseParam.attributes()) {
            if (attrib.scope().has_value()) {
                if (attrib.scope().value() == "xpcf") {
                    setIOType(attrib.name());
                }
            }
        }
    }
    return true;
}

