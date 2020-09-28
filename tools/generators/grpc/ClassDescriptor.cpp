#include "ClassDescriptor.h"
#include <iostream>
#include "MethodDescriptor.h"
#include <cppast/cpp_entity_kind.hpp>

ClassDescriptor::ClassDescriptor(const cppast::cpp_entity& c):m_baseClass(static_cast<const cppast::cpp_class&>(c))
{
}

ClassDescriptor::ClassDescriptor(const cppast::cpp_class& c):m_baseClass(c)
{
}



void ClassDescriptor::generateRpcMapping(const std::map<std::string, std::vector<std::size_t>> & virtualMethodsMap)
{
    for (auto & [name, indexArray] : virtualMethodsMap) {
        if (indexArray.size() > 1) {
            std::size_t methodIndex = 0;
            for (auto & index : indexArray) {
                std::string grpcMethodName = name + "_grpc" + std::to_string(methodIndex++);
                m_virtualMethods[index].m_rpcName = grpcMethodName;
            }
        }
        else {
            m_virtualMethods[indexArray[0]].m_rpcName = name;
        }
    }
}

bool ClassDescriptor::parse(const cppast::cpp_entity_index& index)
{
    std::map<std::string, std::vector<std::size_t>> virtualMethodsMap;
    std::cout << " ==> parsing class "<<m_baseClass.name()<<" scope "<<'\n';
    if (!m_baseClass.attributes().empty()) {
        // handle attrib
    }

    if (cppast::is_templated(m_baseClass)) { // template class is ignored : not part of services : need an instantiation
        return false;
    }

    for (auto & base: m_baseClass.bases()) {
        std::cout<<"Base name="<<base.name()<<std::endl;
        if (base.scope_name().has_value()) {
            std::cout<<"Base scope_name="<<base.scope_name().value().name()<<std::endl;
        }
    }

    for (auto & m : m_baseClass) {
        if (m.kind() == cppast::cpp_entity_kind::member_function_t) {
            // cast to member_function
            MethodDescriptor desc(m);
            desc.parse(index);
            if (desc.isPureVirtual()) {
                m_virtualMethods.push_back(desc);
                virtualMethodsMap[desc.getName()].push_back(m_virtualMethods.size() - 1);
            }
        }
    }

    m_isInterface = (m_virtualMethods.size() > 0);// && ICompIns is ancestor direct or indirect
    if (isInterface()) {
        generateRpcMapping(virtualMethodsMap);
    }
    else {
        // search for xpcf::ISerializable inheritance
        // otherwise fall offs
    }
    //TODO : check ICompIntros is in inheritance tree !!
    return true;
}
