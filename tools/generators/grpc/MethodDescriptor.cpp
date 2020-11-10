#include "MethodDescriptor.h"
#include <iostream>
#include <map>

namespace xpcf =  org::bcom::xpcf;

MethodDescriptor::MethodDescriptor(const cppast::cpp_member_function& m):m_baseMethod(m),m_returnDescriptor(m.return_type())
{
}

const std::map<std::string,MethodDescriptor::streaming_type> str2typeStreamingMap =
{
    {"streaming",MethodDescriptor::streaming_type::bidir},
    {"client_streaming",MethodDescriptor::streaming_type::client},
    {"server_streaming",MethodDescriptor::streaming_type::server},
};

void MethodDescriptor::setStreamingType(const std::string & streamType)
{
    if (str2typeStreamingMap.find(streamType) != str2typeStreamingMap.end()) {
        if (((m_rpcStreamingType == streaming_type::client) && (str2typeStreamingMap.at(streamType) == streaming_type::server)) ||
                ((m_rpcStreamingType == streaming_type::server) && (str2typeStreamingMap.at(streamType) == streaming_type::client))) {
            m_rpcStreamingType = streaming_type::bidir;
        }
        else {
            m_rpcStreamingType = str2typeStreamingMap.at(streamType);
        }
    }
}

std::string MethodDescriptor::getReturnType() const
{
    //TODO : full type must be constructed inside the TypeDescriptor to properly handle template types for instance std::map<T> must become
    // std::map<namespace::T> which can be done only inside the TypeDescriptor
    std::string ret = m_returnType;
    std::string nSpace = m_returnDescriptor.getNamespace();
    if (!nSpace.empty() && nSpace.length() > 0) {
        ret = nSpace + "::" + m_returnType;
    }
    return ret;
}

const std::string MethodDescriptor::getDeclaration() const
{
    std::string methodDeclaration = m_baseMethod.name() + "(";
    uint32_t nbTypes = 0;
    for (auto & p : m_params) {
        if (nbTypes != 0) {
            methodDeclaration += ", ";
        }
        std::string nSpace = p->getNamespace();
        if (!nSpace.empty() && nSpace.length() > 0) {
            nSpace += "::";
        }
        //TODO : full type must be constructed inside the TypeDescriptor to properly handle template types for instance std::map<T> must become
        // std::map<namespace::T> which can be done only inside the TypeDescriptor
        methodDeclaration += nSpace + cppast::to_string(p->getCppastType()) + " " + p->getName();
        nbTypes++;
    }
    methodDeclaration += ")";
    return methodDeclaration;
}

bool MethodDescriptor::parse(const cppast::cpp_entity_index& index)
{
    std::cout << " ==> parsing member function "<<m_baseMethod.name()<<" "<<m_baseMethod.signature()<<'\n';
    cppast::cpp_cv cv = m_baseMethod.cv_qualifier();
    m_const = ((cv == cppast::cpp_cv::cpp_cv_const) || (cv == cppast::cpp_cv::cpp_cv_const_volatile));
    if (!m_baseMethod.attributes().empty()) {
        // handle attrib
        for (auto & attrib : m_baseMethod.attributes()) {
            if (attrib.scope().has_value()) {
                if (attrib.scope().value() == "xpcf") {
                    if (attrib.name() == "ignore") {
                        m_ignored = true;
                        return true;
                    }
                }
                if (attrib.scope().value() == "grpc") {
                    if (attrib.name() == "request") {
                        if (attrib.arguments().has_value()) {
                            m_requestName = attrib.arguments().value().as_string();
                        }
                    }
                    else if (attrib.name() == "response") {
                        if (attrib.arguments().has_value()) {
                            m_responseName = attrib.arguments().value().as_string();
                        }
                    }
                    else {
                        setStreamingType(attrib.name());
                    }
                }

            }
        }
    }

    if (cppast::is_templated(m_baseMethod)) { // template method is ignored : not part of the final service interface : need an instantiation
        return false;
    }

    m_pureVirtual = m_baseMethod.is_virtual() && is_pure(m_baseMethod.virtual_info());
    m_returnType = cppast::to_string(m_baseMethod.return_type());

    for (auto & p : m_baseMethod.parameters()) {
        SRef<ParameterDescriptor> desc  = xpcf::utils::make_shared<ParameterDescriptor>(p);
        desc->parse(index);
        m_params.push_back(desc);

        switch (desc->ioType()) {
        case ParameterDescriptor::io_type::in :
            m_inParams.push_back(desc.get());
            break;
        case ParameterDescriptor::io_type::out :
            m_outParams.push_back(desc.get());
            break;
        case ParameterDescriptor::io_type::inout :
            m_inoutParams.push_back(desc.get());
            break;
        default: // none case
            if (desc->type().isConst()) { // constant parameter means input, either refed or not
                m_inParams.push_back(desc.get());
            }
            if (!desc->type().isReference() && !desc->type().isConst()) { // copy by value : in parameter
                m_inParams.push_back(desc.get());
            }
            if (desc->type().isReference() && !desc->type().isConst()) { // copy by non const ref : inout parameter
                m_inoutParams.push_back(desc.get());
            }
            break;
        }
    }
    m_returnDescriptor.parse(index, m_baseMethod.return_type());
    if (m_inParams.size() > 0) {
        std::cout<<" ==> End parsing member function : found "<<m_inParams.size()<<" input parameters"<<std::endl;
    }
    if ((m_inParams.size() == 0) && (m_inoutParams.size() == 0)) {
        // TODO : fwd empty rpc type from rpc gen stage !
        m_requestName = "::google::protobuf::Empty";
    }
    if ((m_outParams.size() == 0) && (m_inoutParams.size() == 0) && m_returnDescriptor.isVoid()) {
        // TODO : fwd empty rpc type from rpc gen stage !
        m_responseName = "::google::protobuf::Empty";
    }
    std::cout<<" ==> Method declaration "<<getDeclaration()<<std::endl;
    return true;
}
