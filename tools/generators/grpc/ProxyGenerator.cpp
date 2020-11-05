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

void ProxyGenerator::generateHeader(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata, std::ostream& out)
{
    CppBlockManager blockMgr(out);
    //NOTE : proxy is configurable to set grpc channel etc...
    xpcf::uuids::uuid proxyUUID = c->getClientUUID();
    if (proxyUUID.is_nil()) {
        xpcf::uuids::random_generator gen;
        proxyUUID = gen();
        m_serviceUuidMap[m_className] = xpcf::uuids::to_string(proxyUUID);
    }
    blockMgr.out() << "// GRPC Proxy Class Header generated with xpcf_grpc_gen\n\n";
    blockMgr.newline();
    blockMgr.includeGuardsStart(m_className);
    blockMgr.include(c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACEFILEPATH)); // relative or absolute path?? TODO:  retrieve filepath from metadata
    blockMgr.include("xpcf/component/ConfigurableBase.h",false);
    blockMgr.include("memory",false);
    blockMgr.include("string",false);
    blockMgr.include(m_grpcClassName + ".grpc.pb.h");
    blockMgr.include("grpc/grpc.h",false);
    blockMgr.include("grpc++/channel.h",false);

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
            block_guard<CPP::CLASS> classBlk(blockMgr);
            {
                block_guard<CPP::PUBLIC> publicBlk(blockMgr);

                blockMgr.out() << m_className +"();\n";
                blockMgr.out() << "~"+ m_className +"() override = default;\n";
                blockMgr.out() << "void unloadComponent () override final;\n";
                blockMgr.out() << "org::bcom::xpcf::XPCFErrorCode onConfigured() override;\n";
                blockMgr.newline();
                // foreach method
                for (auto m : c->methods()) {
                    blockMgr.out() <<  m->getFullDeclaration();
                    if (m->isConst()) {
                        blockMgr.out() <<  " const";
                    }
                    blockMgr.out() << " override;\n";
                }
                blockMgr.newline();
            }
            {
                block_guard<CPP::PRIVATE> privateBlk(blockMgr);
                blockMgr.out() << "std::string m_channelUrl;\n";
                blockMgr.out() << "uint32_t m_channelCredentials;\n";
                blockMgr.out() << "std::shared_ptr<::grpc::Channel> m_channel;\n";
                blockMgr.out() << "std::unique_ptr<" + m_grpcClassName + "::Stub> m_grpcStub;\n";
            }
        }
    }
    blockMgr.newline();

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
    if (p.type().kind() == type_kind::builtin_t) {
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
    if (p.type().kind() == type_kind::builtin_t) {
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
        blockMgr.out() << "typedef enum ";
        {
            block_guard enumBlk(blockMgr);
            blockMgr.out() << "GoogleDefaultCredentials = 0,\n";
            blockMgr.out() << "SslCredentials = 1,\n";
            blockMgr.out() << "InsecureChannelCredentials = 2\n";
        }
        blockMgr.out() << " grpcCredentials;\n";

        blockMgr.out() << "inline std::shared_ptr< ::grpc::ChannelCredentials > getCredentials(grpcCredentials channelCredentials)\n";
        {
            block_guard methodBlk(blockMgr);
            blockMgr.out() << "switch (channelCredentials) {\n";
            blockMgr.out() << "case GoogleDefaultCredentials:\n";
            blockMgr.out() << "return ::grpc::GoogleDefaultCredentials();\n";
            blockMgr.out() << "case SslCredentials:\n";
            blockMgr.out() << "return ::grpc::SslCredentials(::grpc::SslCredentialsOptions());\n";
            blockMgr.out() << "case InsecureChannelCredentials:       \n";
            blockMgr.out() << "return ::grpc::InsecureChannelCredentials();\n";
            blockMgr.out() << "default:   \n";
            blockMgr.out() << "return nullptr;\n";
            blockMgr.out() << "}\n";
            blockMgr.out() << "return nullptr;\n";
        }
        blockMgr.newline();
        blockMgr.out() << m_className + "::" + m_className + "():xpcf::ConfigurableBase(xpcf::toMap<"+ m_className + ">())\n";
        {
            block_guard methodBlk(blockMgr);
            blockMgr.out() << "declareInterface<" +  c->getName() + ">(this);\n";
            blockMgr.out() << "declareProperty(\"channelUrl\",m_channelUrl);\n";
            blockMgr.out() << "declareProperty(\"channelCredentials\",m_channelCredentials);\n";
        }
        blockMgr.newline();
        blockMgr.out() << "void " + m_className + "::unloadComponent ()\n";
        {
            block_guard methodBlk(blockMgr);
            blockMgr.out() << "delete this;\n";
            blockMgr.out() << "return;\n";
        }
        blockMgr.newline();
        blockMgr.out() << "XPCFErrorCode " + m_className +"::onConfigured()\n";
        {
            block_guard methodBlk(blockMgr);
            blockMgr.out() << "m_channel = ::grpc::CreateChannel(m_channelUrl, getCredentials(static_cast<grpcCredentials>(m_channelCredentials)));\n";
            blockMgr.out() << "m_grpcStub =" + m_grpcClassName + "::NewStub(m_channel);\n";
        }
        blockMgr.newline();

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
                if (m->hasInputs()) {
                    for (ParameterDescriptor * p: m->m_inParams) {
                        bindInput(*p, blockMgr);
                    }
                    for (ParameterDescriptor * p: m->m_inoutParams) {
                        bindInput(*p, blockMgr);
                    }
                }

                blockMgr.out() << "::grpc::Status status = m_grpcStub->" + m->m_rpcName + "(&context, reqIn, &respOut);\n";

                blockMgr.out() << "if (!status.ok())";
                {
                    block_guard condBlk(blockMgr);
                    blockMgr.out() << "std::cout << \"" + m->m_rpcName + "rpc failed.\" << std::endl;\n";
                    blockMgr.out() << "throw xpcf::RemotingException(\"" << m_grpcClassName <<"\",\""<< m->m_rpcName <<"\",static_cast<uint32_t>(status.error_code()));\n";//TODO : differentiate semantic return type from status return type : provide status type name ?
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
                        blockMgr.out() << "return xpcf::deserialize<" << m->getReturnType() << ">(respOut.xpcfgrpcreturnvalue());\n";
                    }
                }
            }
            blockMgr.newline();
        }
    }
}

std::map<IRPCGenerator::MetadataType,std::string> ProxyGenerator::generateImpl(SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata)

{
    m_nameSpace =  "org::bcom::xpcf::grpc::proxy::" + c->getName();
    m_className = c->getName() + "_grpcProxy";
    m_headerFileName = m_className + ".h";
    m_cppFileName = m_className + ".cpp";
    m_grpcClassName = metadata.at(MetadataType::GRPCSERVICENAME);
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

