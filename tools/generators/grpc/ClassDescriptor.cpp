#include "ClassDescriptor.h"
#include <iostream>
#include "MethodDescriptor.h"
#include <cppast/cpp_entity_kind.hpp>
#include <xpcf/core/helpers.h>


namespace xpcf = org::bcom::xpcf;

ClassDescriptor::ClassDescriptor(const cppast::cpp_entity& c, const std::string & nameSpace, const std::string & filePath):m_baseClass(static_cast<const cppast::cpp_class&>(c))
{
    m_metadata[MetadataType::INTERFACENAMESPACE] = nameSpace;
    m_metadata[MetadataType::INTERFACEFILEPATH] = filePath;
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
                m_virtualMethods[index]->m_rpcName = grpcMethodName;
            }
        }
        else {
            m_virtualMethods[indexArray[0]]->m_rpcName = name;
        }
    }
}

void ClassDescriptor::parseMethods(const cppast::cpp_class & c, std::map<std::string, std::vector<std::size_t>> & virtualMethodsMap, const cppast::cpp_entity_index& index)
{
    for (auto & base: c.bases()) {
         std::cout<<"parseMethods::Base name="<<base.name()<<std::endl;
        auto  baseClassRef = cppast::get_class(index,base);
        if (baseClassRef.has_value()) {
            auto & baseClass = baseClassRef.value();
            // avoid to add twice methods from diamond interface inheritance.
            // WARNING: reminder : may have a conflict as the class name is not namespaced here !
            if (!xpcf::mapContains(m_classParsed,baseClass.name())) {
                m_classParsed[baseClass.name()] = true;
                parseMethods(baseClass, virtualMethodsMap, index);
            }
        }
    }
    for (auto & m : c) {
        if (m.kind() == cppast::cpp_entity_kind::member_function_t) {
            // cast to member_function
            SRef<MethodDescriptor> desc  = xpcf::utils::make_shared<MethodDescriptor>(static_cast<const cppast::cpp_member_function&>(m));
            desc->parse(index);
            if (desc->isPureVirtual() && !desc->ignored()) {
                m_virtualMethods.push_back(desc);
                virtualMethodsMap[desc->getName()].push_back(m_virtualMethods.size() - 1);
            }
        }
    }
}

bool ClassDescriptor::parse(const cppast::cpp_entity_index& index)
{
    std::map<std::string, std::vector<std::size_t>> virtualMethodsMap;
    std::cout << " ==> parsing class "<<m_baseClass.name()<<" scope "<<std::endl;
    if (!m_baseClass.attributes().empty()) {
        // handle attrib
        for (auto & attrib : m_baseClass.attributes()) {
            if (attrib.scope().has_value()) {
                if (attrib.scope().value() == "xpcf") {
                    if (attrib.name() == "ignore") {
                        m_ignored = true;
                        return true;
                    }
                    if (attrib.name() == "clientUUID") {
                        if (attrib.arguments().has_value()) {
                            m_clientUUID = xpcf::toUUID(attrib.arguments().value().as_string());
                        }
                    }
                    if (attrib.name() == "serverUUID") {
                        if (attrib.arguments().has_value()) {
                            m_serverUUID = xpcf::toUUID(attrib.arguments().value().as_string());
                        }
                    }
                }
            }
        }
    }

    if (cppast::is_templated(m_baseClass)) { // template class is ignored : not part of services : need an instantiation
        return false;
    }

    for (auto & base: m_baseClass.bases()) {
        std::cout<<"Base name="<<base.name()<<std::endl;
        m_bases.push_back(base.name());
        if (base.scope_name().has_value()) {
            std::cout<<"Base scope_name="<<base.scope_name().value().name()<<std::endl;
        }
    }

    parseMethods(m_baseClass,virtualMethodsMap,index);

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
