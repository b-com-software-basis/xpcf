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

void ProxyGenerator::generateHeader(const ClassDescriptor & c, std::ostream& out)
{
    //NOTE : proxy is configurable to set grpc channel etc...
    xpcf::uuids::random_generator gen;
    xpcf::uuids::uuid proxyUUID = gen();

    out << "// GRPC Proxy Class Header generated with xpcf_grpc_gen\n\n";
    newline(out);
    includeGuardsStart(m_className, out);
    out<< "#include \"" + c.getName() +".h\"\n"; // relative or absolute path?? TODO:  retrieve filepath from metadata
    out << "#include <xpcf/component/ConfigurableBase.h>\n";
    out << "#include <memory>\n";
    out << "#include <string>\n";
    newline(out);
    out << "namespace " + m_nameSpace +" {\n";

    out << "class " + m_className + ":  public org::bcom::xpcf::ConfigurableBase, virtual public " + c.getName() + " {\n";
    out << "public:\n";
    out << indent() << m_className +"();\n";
    out << indent() << "~"+ m_className +"() override = default;\n";
    out << indent() << "void unloadComponent () override final;\n";
    out << indent() << "XPCFErrorCode onConfigured() override;\n";
    newline(out);
    // foreach method
    for (auto m : c.methods()) {
        out << indent() <<  m.getFullDeclaration() + " override;\n";
    }
    newline(out);
    out << "private:\n";
    out << indent() << "std::string m_channelUrl;\n";
    out << indent() << "uint32_t m_channelCredentials;\n";
    out << indent() << "std::shared_ptr<grpc::Channel> m_channel;\n";
    out << indent() << "std::unique_ptr<" + m_grpcClassName + "::Stub> m_grpcStub;\n";
    out << "}; //end proxy class\n";
    out << "}//end namespace\n";
    newline(out);

    out << "template <> struct org::bcom::xpcf::ComponentTraits<" + m_nameSpace + "::" + m_className +">\n";//xpcf::grpc::proxy::c.name::c.name_grpcProxy>
    out << "{\n";
    out << indent() << "static constexpr const char * UUID = \"" + xpcf::uuids::to_string(proxyUUID) + "\";\n";
    out << indent() << "static constexpr const char * NAME = \"" + m_className + "\";\n";
    out << indent() << "static constexpr const char * DESCRIPTION = \"" + m_className + " grpc client proxy component\";\n";
    out << "};\n";
    newline(out);
    includeGuardsEnd(out);
}

void ProxyGenerator::generateBody(const ClassDescriptor & c, std::ostream& out)
{
    out << "// GRPC Proxy Class implementation generated with xpcf_grpc_gen\n";
    out << "#include \"" + m_headerFileName + "\"\n";
    out << "#include <cstddef>\n";
    // the body will use the grpcCLient generated from the proto or flat generators hence the following inclusion :
    //#include "grpcgeneratedclient.grpc.[pb|fb].h"





    out << "namespace xpcf = org::bcom::xpcf;\n";
    newline(out);

    out << "template<> " + m_nameSpace + "::" + m_className + "* xpcf::ComponentFactory::createInstance<"+ m_nameSpace + "::" + m_className + ">();\n";
    newline(out);
    out << "namespace " + m_nameSpace + cppBeginBlock;
    newline(out);
    out << "typedef enum " + cppBeginBlock;
    out << indent() << "GoogleDefaultCredentials = 0,\n";
    out << indent() << "SslCredentials = 1,\n";
    out << indent() << "InsecureChannelCredentials = 2\n";
    out << cppEndBlock + " grpcCredentials;\n";

    out << "inline std::shared_ptr< grpc::ChannelCredentials > getCredentials(grpcCredentials channelCredentials)\n";
    out << cppBeginBlock;
    out << indent() << "switch (channelCredentials) {\n";
    out << indent() << "case GoogleDefaultCredentials:\n";
    out << indent(2) << "return grpc::GoogleDefaultCredentials();\n";
    out << indent() << "case SslCredentials:\n";
    out << indent(2) << "return grpc::SslCredentials(grpc::SslCredentialsOptions());\n";
    out << indent() << "case InsecureChannelCredentials:       \n";
    out << indent(2) << "return grpc::InsecureChannelCredentials();\n";
    out << indent() << "default:   \n";
    out << indent(2) << "return nullptr;\n";
    out << indent() << "}\n";
    out << indent() << "return nullptr;\n";
    out << cppEndBlock;
    newline(out);
    out << m_className + "::" + m_className + "():xpcf::ConfigurableBase(xpcf::toMap<"+ m_className + ">())\n";
    out << cppBeginBlock;
    out << "declareInterface<" +  c.getName() + ">(this);\n";
    out << "declareProperty(\"channelUrl\",m_channelUrl);\n";
    out << "declareProperty(\"channelCredentials\",m_channelCredentials);\n";
    out << cppEndBlock;
    newline(out);
    out << "void " + m_className + "::unloadComponent ()\n";
    out << cppBeginBlock;
    out << "delete this;\n";
    out << "return;\n";
    out << cppEndBlock;
    newline(out);
    out << "XPCFErrorCode " + m_className +"::onConfigured()\n";
    out << cppBeginBlock;
    out << "m_channel = grpc::CreateChannel(m_channelUrl, getCredentials(m_channelCredentials));\n";
    out << "m_grpcStub =" + m_grpcClassName + "::NewStub(m_channel);\n";
    out << cppEndBlock;
    newline(out);

    for (auto m : c.methods()) {
        out << m.getReturnType() + "  "+ m_className + "::" + m.getDeclaration() + "\n";
        out << cppBeginBlock;//missing serialize/deserialize from method cpp params !
        out << "  ClientContext context\n;";
        out << indent() << "Status status = m_grpcStub->" + m.m_rpcName + "(&context, " + m.m_requestName + "," + m.m_responseName + ");\n";
        out << indent() << "if (!status.ok()) {\n";
        out << indent(2) << "std::cout << \"" + m.m_rpcName + "rpc failed.\" << std::endl;\n";
        out << indent(2) << "return static_cast<" + m.getReturnType() + ">(XPCFErrorCode::_FAIL);\n";//TODO : differentiate semantic return type from status return type : provide status type name ?
        out << indent() << "}\n";
        out << "  return static_cast<" + m.getReturnType() + ">(XPCFErrorCode::_SUCCESS);\n";
        out << cppEndBlock;
        newline(out);
    }
    out << cppEndBlock;
}

std::map<IRPCGenerator::MetadataType,std::string> ProxyGenerator::generate(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata)

{
    m_nameSpace =  "xpcf::grpc::proxy::" + c.getName();
    m_className = c.getName() + "_grpcProxy";
    m_headerFileName = m_className + ".h";
    m_cppFileName = m_className + ".cpp";
    m_grpcClassName = metadata.at(MetadataType::GRPCSERVICENAME);
    //m_grpcClassName = ??
    if (m_mode == GenerateMode::STD_COUT) {
        generateHeader(c, std::cout);
        generateBody(c, std::cout);
        std::cout<<std::endl;
    }
    else {
        fs::detail::utf8_codecvt_facet utf8;
        fs::path headerFilePath(m_headerFileName,utf8);
        fs::path cppFilePath(m_cppFileName,utf8);
        headerFilePath = m_folder/headerFilePath;
        cppFilePath = m_folder/cppFilePath;
        std::ofstream headerFile(headerFilePath.generic_string(utf8).c_str(), std::ios::out);
        std::ofstream cppFile(cppFilePath.generic_string(utf8).c_str(), std::ios::out);
        generateHeader(c, headerFile);
        generateBody(c, cppFile);
    }
    return metadata;
}

