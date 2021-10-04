#include "ClassDescriptor.h"
#include <iostream>
#include "MethodDescriptor.h"
#include <cppast/cpp_entity_kind.hpp>
#include <xpcf/core/helpers.h>
#include <boost/algorithm/string.hpp>


namespace xpcf = org::bcom::xpcf;

ClassDescriptor::ClassDescriptor(const cppast::cpp_entity& c, const std::string & nameSpace, const std::string & filePath):m_baseClass(static_cast<const cppast::cpp_class&>(c))
{
    m_metadata[MetadataType::INTERFACENAMESPACE] = nameSpace;
    m_metadata[MetadataType::INTERFACEFILEPATH] = filePath;
}

ClassDescriptor::ClassDescriptor(const cppast::cpp_class& c):m_baseClass(c)
{
}

std::string ClassDescriptor::getFullName() const
{
    if (m_metadata.at(MetadataType::INTERFACENAMESPACE).empty()) {
        return m_baseClass.name();
    }
    return m_metadata.at(MetadataType::INTERFACENAMESPACE) + "::" + m_baseClass.name();
}

void ClassDescriptor::setXpcfTrait(const org::bcom::xpcf::Traits & trait)
{
    m_xpcfTrait.uuid = trait.uuid;
    m_xpcfTrait.name = trait.name;
    m_xpcfTrait.description = trait.description;
}

const org::bcom::xpcf::uuids::uuid & ClassDescriptor::createClientUUID() const
{
    xpcf::uuids::random_generator gen;
    m_clientUUID = gen();
    return m_clientUUID;
}

const org::bcom::xpcf::uuids::uuid & ClassDescriptor::createServerUUID() const
{
    xpcf::uuids::random_generator gen;
    m_serverUUID = gen();
    return m_serverUUID;
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
        if (base.name() == "org::bcom::xpcf::ComponentBase" ||
                base.name() == "org::bcom::xpcf::ConfigurableBase") {
            m_isXpcfComponent = true;
        }
        if (base.name() == "org::bcom::xpcf::IComponentIntrospect") {
            m_isXpcfInterface = true;
        }
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
            if (desc->isPureVirtual()) {
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
                            std::string clientUUIDStr =  attrib.arguments().value().as_string();
                            boost::algorithm::erase_all(clientUUIDStr,"\"");
                            m_clientUUID = xpcf::toUUID(clientUUIDStr);
                        }
                    }
                    if (attrib.name() == "serverUUID") {
                        if (attrib.arguments().has_value()) {
                            std::string serverUUIDStr =  attrib.arguments().value().as_string();
                            boost::algorithm::erase_all(serverUUIDStr,"\"");
                            m_serverUUID = xpcf::toUUID(serverUUIDStr);
                        }
                    }
                }
                if (attrib.scope().value() == "grpc") {
                    if (attrib.name() == "client_receiveSize") {
                        if (attrib.arguments().has_value()) {
                            //when no attrib : howto figure out to not specify channel arguments ?
                            std::string recvSizeStr = attrib.arguments().value().as_string();
                            boost::algorithm::erase_all(recvSizeStr,"\"");
                            m_clientReceiveSizeOverriden = true;
                            m_clientReceiveSize = std::atol(recvSizeStr.c_str());
                        }
                    }
                    else if (attrib.name() == "client_sendSize") {
                        if (attrib.arguments().has_value()) {
                            std::string sndSizeStr = attrib.arguments().value().as_string();
                            boost::algorithm::erase_all(sndSizeStr,"\"");
                            m_clientSendSizeOverriden = true;
                            m_clientSendSize = std::atol(sndSizeStr.c_str());
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

    m_isInterface = !m_isXpcfComponent && (m_virtualMethods.size() > 0) && m_isXpcfInterface;// && ICompIns is ancestor direct or indirect
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
