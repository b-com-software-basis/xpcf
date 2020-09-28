#include "ServerGenerator.h"
#include "CppHelpers.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace xpcf = org::bcom::xpcf;

template<> ServerGenerator * xpcf::ComponentFactory::createInstance<ServerGenerator>();

ServerGenerator::ServerGenerator():AbstractGenerator(xpcf::toMap<ServerGenerator>())
{
}


ServerGenerator::~ServerGenerator()
{

}

void ServerGenerator::generateHeader(const ClassDescriptor & c, std::ostream& out)
{
    //NOTE : proxy is configurable to set grpc channel etc...
    xpcf::uuids::random_generator gen;
    xpcf::uuids::uuid proxyUUID = gen();

    out << "// GRPC Server Class Header generated with xpcf_grpc_gen\n";
    newline(out);
    includeGuardsStart(m_className, out);
    out<< "#include \"" + c.getName() +".h\"\n"; // relative or absolute path?? TODO:  retrieve filepath from metadata
    out << "#include <xpcf/component/ConfigurableBase.h>\n";
    out << "#include <xpcf/remoting/IGrpcService.h>\n";
    newline(out);
    out << "namespace " + m_nameSpace +" {\n";

    out << "class " + m_className + ":  public org::bcom::xpcf::ConfigurableBase, virtual public org::bcom::xpcf::IGrpcService {\n";
    out << "public:\n";
    out << "  " + m_className +"();\n";
    out << "  ~"+ m_className +"() override = default;\n";
    out << "  grpc::Service * getService() override;\n";
    out << "  void unloadComponent () override final;\n";
    out << "  XPCFErrorCode onConfigured() override;\n";
    newline(out);
    out << "class " + m_grpcClassName + "Impl:  public "+ m_grpcClassName+"::Service {\n";
    out << "  " + m_grpcClassName + "Impl() = default;\n";// missing something to set the ref toward the injected I* component
    // foreach method
    for (auto m : c.methods()) {
        out << "  Status " + m.m_rpcName + "(ServerContext* context, " + m.m_requestName + ", " + m.m_responseName + ") override;\n";
    }
    out << "};\n";
    newline(out);
    out << "private:\n";
    out << "  "+ m_grpcClassName + "Impl m_grpcService;\n";
    out << "  SRef<" + c.getName() + "> m_xpcfComponent;\n";
    out << "}; //end service class\n";
    out << "}//end namespace\n";
    newline(out);
    out << "template <> struct org::bcom::xpcf::ComponentTraits<" + m_nameSpace + "::" + m_className +">\n";//xpcf::grpc::proxy::c.name::c.name_grpcProxy>
    out << "{\n";
    out << " static constexpr const char * UUID = \"" + xpcf::uuids::to_string(proxyUUID) + "\";\n";
    out << " static constexpr const char * NAME = \"" + m_className + "\";\n";
    out << "  static constexpr const char * DESCRIPTION = \"" + m_className + " grpc server component\";\n";
    out << "};\n";

    includeGuardsEnd(out);
}

void ServerGenerator::generateBody(const ClassDescriptor & c, std::ostream& out)
{
    out << "// GRPC Server Class implementation generated with xpcf_grpc_gen\n";
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
    out << m_className + "::" + m_className + "()\n";
    out << cppBeginBlock;
    out << "declareInterface<xpcf::IGrpcService>(this);\n";
    out << "declareInjectable<" +  c.getName() + ">(m_xpcfComponent);\n";
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
    out << cppEndBlock;
    newline(out);
    out << "grpc::Service * " + m_className +"::getService()\n";
    out << cppBeginBlock;
    out << "return &m_grpcService;\n";
    out << cppEndBlock;

    for (auto m : c.methods()) {
        out << "Status "+ m_grpcClassName + "Impl()::" + m.m_rpcName + "(ServerContext* context,"+ m.m_requestName+", "+m.m_responseName+")\n";
        out << cppBeginBlock;

        out << cppEndBlock;
    }
    out << cppEndBlock;
}

std::map<IRPCGenerator::MetadataType,std::string> ServerGenerator::generate(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata)
{
    m_nameSpace =  "xpcf::grpc::server::" + c.getName();
    m_className = c.getName() + "_grpcServer";
    m_headerFileName = m_className + ".h";
    m_cppFileName = m_className + ".cpp";
    m_grpcClassName = metadata.at(MetadataType::GRPCSERVICENAME);
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
