#include "ProxyGenerator.h"
#include "CppHelpers.h"

#include <fstream>

namespace xpcf = org::bcom::xpcf;

template<> ProxyGenerator * xpcf::ComponentFactory::createInstance<ProxyGenerator>();


ProxyGenerator::ProxyGenerator():AbstractGenerator(xpcf::toMap<ProxyGenerator>())
{
}


ProxyGenerator::~ProxyGenerator()
{

}

void ProxyGenerator::processHeaderMethods(const SRef<ClassDescriptor> c, CppBlockManager & blockMgr)
{
    for (auto m : c->methods()) {
        blockMgr.out() <<  m->getFullDeclaration();
        if (m->isConst()) {
            blockMgr.out() <<  " const";
        }
        blockMgr.out() << " override;\n";
        if (m->returnType().kind() == type_kind::c_string_t) {
            m_proxyMembersVariables["m_" + m->getName()] = std::make_pair("std::string", false);
        }
        else if (m->returnType().isConst() && m->returnType().isReference()) {
            m_proxyMembersVariables["m_" + m->getName()] = std::make_pair(m->returnType().getFullTypeDescription(),m->isConst());
        }
    }
    blockMgr.newline();
}

void ProxyGenerator::generateHeader(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata, std::ostream& out)
{
    CppBlockManager blockMgr(out);
    //NOTE : proxy is configurable to set grpc channel etc...
    xpcf::uuids::uuid proxyUUID = c->getClientUUID();
    if (proxyUUID.is_nil()) {
        proxyUUID = c->createClientUUID();
        m_serviceUuidMap[m_className] = xpcf::uuids::to_string(proxyUUID);
    }
    blockMgr.out() << "// GRPC Proxy Class Header generated with xpcf_grpc_gen\n\n";
    blockMgr.newline();
    blockMgr.includeGuardsStart(m_className);
    blockMgr.include(c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACEFILEPATH)); // relative or absolute path?? TODO:  retrieve filepath from metadata
    blockMgr.include("xpcf/component/ConfigurableBase.h",false);
    blockMgr.include("memory",false);
    blockMgr.include("string",false);
    blockMgr.include("map",false);
    blockMgr.include((*c)[ClassDescriptor::MetadataType::GRPCSERVICENAME] + ".grpc.pb.h");
    blockMgr.include("grpc/grpc.h",false);
    blockMgr.include("grpc++/channel.h",false);
    blockMgr.include("xpcf/remoting/GrpcHelper.h",false);
    blockMgr.newline();
    blockMgr.out() << "namespace " + m_nameSpace + " ";
    {
        block_guard<CPP::NSPACE> nspaceBlk(blockMgr);
        std::string baseInterface;
        if (!c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE).empty()) {
            baseInterface = c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE) + "::";
        }
        baseInterface += c->getName();
        blockMgr.newline();
        blockMgr.out() << "class " + m_className + ":  public org::bcom::xpcf::ConfigurableBase, virtual public " + baseInterface + " ";
        {
            std::map<std::string,std::pair<std::string,bool>> proxyMembersVariables; // name, {type, isConstMethod}
            block_guard<CPP::CLASS> classBlk(blockMgr);
            {
                block_guard<CPP::PUBLIC> publicBlk(blockMgr);

                blockMgr.out() << m_className +"();\n";
                blockMgr.out() << "~"+ m_className +"() override = default;\n";
                blockMgr.out() << "void unloadComponent () override final;\n";
                blockMgr.out() << "org::bcom::xpcf::XPCFErrorCode onConfigured() override;\n";
                blockMgr.newline();
                // foreach method
                processHeaderMethods(c, blockMgr);
            }
            {
                block_guard<CPP::PRIVATE> privateBlk(blockMgr);
                blockMgr.out() << "std::string m_channelUrl;\n";
                blockMgr.out() << "uint32_t m_channelCredentials;\n";
                blockMgr.out() << "std::shared_ptr<::grpc::Channel> m_channel;\n";
                blockMgr.out() << "xpcf::grpcCompressionInfos m_serviceCompressionInfos;\n";
                blockMgr.out() << "std::map<std::string, xpcf::grpcCompressionInfos> m_methodCompressionInfosMap;\n";
                blockMgr.out() << "std::vector<std::string> m_grpcProxyCompressionConfig;\n";

                blockMgr.out() << "std::unique_ptr<::" +  c->getMetadata().at(ClassDescriptor::MetadataType::REMOTINGNSPACE) + "::" + m_grpcClassName + "::Stub> m_grpcStub;\n";
                for (auto & [name, value ] : m_proxyMembersVariables) {
                    auto & [type, isConst] = value;
                            if (isConst) {
                        blockMgr.out() << "mutable ";
                    }
                    blockMgr.out() << type << " " << name << ";\n";
                }
            }
        }
    }
    blockMgr.newline();

    m_proxyMembersVariables.clear();

    blockMgr.out() << "template <> struct org::bcom::xpcf::ComponentTraits<" + m_nameSpace + "::" + m_className +">\n";//xpcf::grpc::proxy::c.name::c.name_grpcProxy>
    {
        block_guard<CPP::CLASS> classBlk(blockMgr);
        blockMgr.out() << "static constexpr const char * UUID = \"" + xpcf::uuids::to_string(proxyUUID) + "\";\n";
        blockMgr.out() << "static constexpr const char * NAME = \"" + m_className + "\";\n";
        blockMgr.out() << "static constexpr const char * DESCRIPTION = \"" + m_className + " grpc client proxy component\";\n";
    }
    blockMgr.newline();
    blockMgr.includeGuardsEnd();
}

void ProxyGenerator::bindInput(const ParameterDescriptor & p, CppBlockManager & blockMgr)
{
    if (p.type().kind() == type_kind::builtin_t
            || p.type().kind() == type_kind::enum_t
            || p.type().kind() == type_kind::std_string_t
            || p.type().kind() == type_kind::c_string_t) {
        if (p.type().needsStaticCast()) {
            blockMgr.out() << "reqIn.set_"<< boost::to_lower_copy(p.getName()) << "(static_cast<" << p.type().getRPCType() << ">(" << p.getName() << "));\n";
        }
        else {
            blockMgr.out() << "reqIn.set_"<< boost::to_lower_copy(p.getName()) << "(" << p.getName() << ");\n";
        }

    }
    else if ((p.type().kind() == type_kind::user_defined_t) || (p.type().kind() == type_kind::template_t)) {
        blockMgr.out() << "reqIn.set_"<< boost::to_lower_copy(p.getName()) << "(xpcf::serialize<" << p.type().getFullTypeDescription() <<">(" << p.getName() << "));\n";
    }
}

void ProxyGenerator::bindOutput(const ParameterDescriptor & p, CppBlockManager & blockMgr)
{
    if (p.type().kind() == type_kind::builtin_t
            || p.type().kind() == type_kind::enum_t
            || p.type().kind() == type_kind::std_string_t) {
        if (p.type().needsStaticCast()) {
            blockMgr.out() << p.getName() << " = static_cast<" << p.type().getFullTypeDescription() << ">(respOut." << boost::to_lower_copy(p.getName()) <<"());\n";
        }
        else {
            blockMgr.out() <<p.getName()<<" = respOut."<< boost::to_lower_copy(p.getName()) <<"();\n";
        }
    }
    else if ((p.type().kind() == type_kind::user_defined_t)|| (p.type().kind() == type_kind::template_t)) {
        blockMgr.out() <<p.getName()<<" = xpcf::deserialize<" << p.type().getFullTypeDescription() << ">(respOut."<<boost::to_lower_copy(p.getName()) <<"());\n";
    }
}

void ProxyGenerator::processBodyMethods(const SRef<ClassDescriptor> c, CppBlockManager & blockMgr)
{
    for (auto m : c->methods()) {
        blockMgr.out() << m->getReturnType() + "  "+ m_className + "::" + m->getDeclaration();
        if (m->isConst()) {
            blockMgr.out() <<  " const";
        }
        blockMgr.out() << "\n";
        {
            block_guard methodBlk(blockMgr);
            blockMgr.out() << "::grpc::ClientContext context;\n";
            blockMgr.out() << m->m_requestName << " reqIn;\n";
            blockMgr.out() << m->m_responseName << " respOut;\n";
            if (m->hasOutputs()) {
                blockMgr.out() << "xpcf::grpcCompressionInfos serverCompressionInfo = xpcf::deduceClientCompressionInfo(m_serviceCompressionInfos, \""<<m->getName()<<"\", m_methodCompressionInfosMap);\n";
                blockMgr.out() << "xpcf::grpcCompressType serverCompressionType = xpcf::prepareClientCompressionContext(context, serverCompressionInfo);\n";
                blockMgr.out() << "reqIn.set_grpcservercompressionformat (static_cast<int32_t>(serverCompressionType));\n";
            }
            if (m->hasInputs()) {
                for (ParameterDescriptor * p: m->m_inParams) {
                    bindInput(*p, blockMgr);
                }
                for (ParameterDescriptor * p: m->m_inoutParams) {
                    bindInput(*p, blockMgr);
                }
            }

            blockMgr.out() << "::grpc::Status grpcRemoteStatus = m_grpcStub->" + m->m_rpcName + "(&context, reqIn, &respOut);\n";

            blockMgr.out() << "if (!grpcRemoteStatus.ok())";
            {
                block_guard condBlk(blockMgr);
                blockMgr.out() << "std::cout << \"" + m->m_rpcName + " rpc failed.\" << std::endl;\n";
                blockMgr.out() << "throw xpcf::RemotingException(\"" << m_grpcClassName <<"\",\""<< m->m_rpcName <<"\",static_cast<uint32_t>(grpcRemoteStatus.error_code()));\n";//TODO : differentiate semantic return type from status return type : provide status type name ?
            }
            if (m->hasOutputs())  {
                for (ParameterDescriptor * p: m->m_inoutParams) {
                    bindOutput(*p, blockMgr);
                }
                for (ParameterDescriptor * p: m->m_outParams) {
                    bindOutput(*p, blockMgr);
                }
            }
            if (!m->returnType().isVoid()) {
                if (m->returnType().kind() == type_kind::builtin_t) {
                    if (m->returnType().needsStaticCast()) {
                        blockMgr.out() << "return static_cast<" << m->returnType().getFullTypeDescription() << ">(respOut.xpcfgrpcreturnvalue());\n";
                    }
                    else {
                        blockMgr.out() << "return respOut.xpcfgrpcreturnvalue();\n";
                    }
                }
                else if (m->returnType().kind() == type_kind::user_defined_t) {
                    if (m->returnType().isConst() && m->returnType().isReference()) {
                        blockMgr.out() << "m_" << m->getName()<<" = xpcf::deserialize<" << m->returnType().getFullTypeDescription() << ">(respOut.xpcfgrpcreturnvalue());\n";
                        blockMgr.out() << "return m_" << m->getName() << ";\n";
                    }
                    else {
                        blockMgr.out() << "return xpcf::deserialize<" << m->returnType().getFullTypeDescription() << ">(respOut.xpcfgrpcreturnvalue());\n";
                    }
                }
                else if (m->returnType().kind() == type_kind::enum_t) {
                    if (m->returnType().needsStaticCast()) {
                        blockMgr.out() << "return static_cast<" << m->returnType().getFullTypeDescription() << ">(respOut.xpcfgrpcreturnvalue());\n";
                    }
                    else {
                        blockMgr.out() << "return respOut.xpcfgrpcreturnvalue();\n";
                    }
                }
                else if (m->returnType().kind() == type_kind::c_string_t ) {
                    blockMgr.out() << "m_" << m->getName()<<" = respOut.xpcfgrpcreturnvalue();\n";
                    blockMgr.out() << "return m_" << m->getName() << ".c_str();\n";
                }
            }
        }
        blockMgr.newline();
    }
}

void ProxyGenerator::generateBody(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata, std::ostream& out)
{
    CppBlockManager blockMgr(out);
    blockMgr.out() << "// GRPC Proxy Class implementation generated with xpcf_grpc_gen\n";
    blockMgr.include(m_headerFileName);
    blockMgr.include("cstddef",false);
    blockMgr.include("xpcf/core/Exception.h",false);
    blockMgr.include("xpcf/remoting/ISerializable.h",false);
    blockMgr.include("grpcpp/client_context.h",false);
    blockMgr.include("grpcpp/create_channel.h",false);
    blockMgr.include("grpcpp/security/credentials.h",false);
    blockMgr.include("boost/algorithm/string.hpp",false);
    // the body will use the grpcCLient generated from the proto or flat generators hence the following inclusion :
    //#include "grpcgeneratedclient.grpc.[pb|fb].h"
    blockMgr.out() << "namespace xpcf = org::bcom::xpcf;\n";


    blockMgr.newline();

    blockMgr.out() << "template<> " + m_nameSpace + "::" + m_className + "* xpcf::ComponentFactory::createInstance<"+ m_nameSpace + "::" + m_className + ">();\n";
    blockMgr.newline();
    blockMgr.out() << "namespace " + m_nameSpace + " ";
    {
        block_guard<CPP::NSPACE> nspaceBlk(blockMgr);
        blockMgr.newline();

        blockMgr.out() << m_className + "::" + m_className + "():xpcf::ConfigurableBase(xpcf::toMap<"+ m_className + ">())\n";
        {
            block_guard methodBlk(blockMgr);
            std::string baseInterface;
            if (!c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE).empty()) {
                baseInterface = c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE) + "::";
            }
            baseInterface += c->getName();
            blockMgr.out() << "declareInterface<" + baseInterface + ">(this);\n";
            blockMgr.out() << "declareProperty(\"channelUrl\",m_channelUrl);\n";
            blockMgr.out() << "declareProperty(\"channelCredentials\",m_channelCredentials);\n";
            blockMgr.out() << "declarePropertySequence(\"grpc_compress_proxy\", m_grpcProxyCompressionConfig);\n";
        }
        blockMgr.newline();
        blockMgr.out() << "void " + m_className + "::unloadComponent ()\n";
        {
            block_guard methodBlk(blockMgr);
            blockMgr.out() << "delete this;\n";
            blockMgr.out() << "return;\n";
        }
        blockMgr.newline();
        // grpc channel is opened once in onConfigured, as opening the channel has some overhead, it should be maintain as long as possible
        blockMgr.out() << "XPCFErrorCode " + m_className +"::onConfigured()\n";
        {
            block_guard methodBlk(blockMgr);
            if (c->clientReceiveSizeOverriden() || c->clientSendSizeOverriden()) {
                blockMgr.out() << "::grpc::ChannelArguments ch_args;\n";
                if (c->clientReceiveSizeOverriden() ) {
                     blockMgr.out() << "ch_args.SetMaxReceiveMessageSize("<<c->getClientReceiveSize()<<");\n";
                }
                if (c->clientSendSizeOverriden() ) {
                     blockMgr.out() << "ch_args.SetMaxSendMessageSize("<<c->getClientSendSize()<<");\n";
                }
                blockMgr.out() << "m_channel = ::grpc::CreateCustomChannel(m_channelUrl,\n";
                blockMgr.out() << "xpcf::GrpcHelper::getCredentials(static_cast<xpcf::grpcCredentials>(m_channelCredentials)),\n";
                blockMgr.out() << "ch_args);\n";
            }
            else {
                blockMgr.out() << "m_channel = ::grpc::CreateChannel(m_channelUrl, xpcf::GrpcHelper::getCredentials(static_cast<xpcf::grpcCredentials>(m_channelCredentials)));\n";
            }
            blockMgr.out() << "m_grpcStub = ::" + c->getMetadata().at(ClassDescriptor::MetadataType::REMOTINGNSPACE) + "::" + m_grpcClassName + "::NewStub(m_channel);\n";
            blockMgr.out() << "for (auto & compressionLine : m_grpcProxyCompressionConfig) {\n";
            blockMgr.out() << "    translateClientConfiguration(compressionLine, m_serviceCompressionInfos, m_methodCompressionInfosMap);\n";
            blockMgr.out() << "}\n";

            blockMgr.out() << "return xpcf::XPCFErrorCode::_SUCCESS;\n";
        }
        blockMgr.newline();
        processBodyMethods(c, blockMgr);
    }
}

std::map<IRPCGenerator::MetadataType,std::string> ProxyGenerator::generateImpl(SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata)

{
    m_nameSpace =  "org::bcom::xpcf::grpc::proxy" + c->getName();
    m_className = c->getName() + "_grpcProxy";
    m_headerFileName = m_className + ".h";
    m_cppFileName = m_className + ".cpp";
    m_grpcClassName = (*c)[ClassDescriptor::MetadataType::GRPCSERVICENAME];
    //m_grpcClassName = ??
    if (m_mode == GenerateMode::STD_COUT) {
        generateHeader(c, metadata, std::cout);
        generateBody(c, metadata, std::cout);
        std::cout<<std::endl;
    }
    else {
        fs::detail::utf8_codecvt_facet utf8;
        fs::path headerFilePath(m_headerFileName,utf8);
        fs::path cppFilePath(m_cppFileName,utf8);
        headerFilePath = m_folder/headerFilePath;
        cppFilePath = m_folder/cppFilePath;
        std::ofstream headerFile(headerFilePath.generic_string(utf8).c_str(), std::ios::out);
        generateHeader(c, metadata, headerFile);
        headerFile.close();
        std::ofstream cppFile(cppFilePath.generic_string(utf8).c_str(), std::ios::out);
        generateBody(c, metadata, cppFile);
        cppFile.close();
    }
    metadata[PROXY_XPCFGRPCCOMPONENTNAME] = m_className;
    metadata[PROXY_XPCFGRPCNAMESPACE] = m_nameSpace;
    metadata[MetadataType::PROXY_HEADERFILENAME] = m_headerFileName;
    metadata[MetadataType::PROXY_CPPFILENAME] = m_cppFileName;
    return metadata;
}

void ProxyGenerator::finalizeImpl(std::map<MetadataType,std::string> metadata)
{
    for (auto [name, uuid] : m_serviceUuidMap) {
        std::cout << "Component " << name << " generated uuid = [[xpcf::clientUUID(" << uuid <<")]]" << std::endl;
    }
    m_serviceUuidMap.clear();
}

