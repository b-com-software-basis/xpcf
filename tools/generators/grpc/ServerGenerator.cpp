#include "ServerGenerator.h"
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

void ServerGenerator::processHeaderMethods(const SRef<ClassDescriptor> c, CppBlockManager & blockMgr)
{
    for (auto m : c->methods()) {
        std::string request = m->m_requestName;
        std::string response = m->m_responseName;
        blockMgr.out() << "::grpc::Status " + m->m_rpcName + "(::grpc::ServerContext* context, const " + request + "* request, " + response + "* response) override;\n";
    }
    blockMgr.newline();
}


void ServerGenerator::generateHeader(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata, std::ostream& out)
{
    //NOTE : server is configurable to set grpc channel etc...
    xpcf::uuids::uuid serverUUID = c->getServerUUID();
    if (serverUUID.is_nil()) {
        xpcf::uuids::random_generator gen;
        serverUUID = gen();
        m_serviceUuidMap[m_className] = xpcf::uuids::to_string(serverUUID);
    }

    CppBlockManager blockMgr(out);
    blockMgr.out() << "// GRPC Server Class Header generated with xpcf_grpc_gen\n";
    blockMgr.newline();
    blockMgr.includeGuardsStart(m_className);
    blockMgr.include(c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACEFILEPATH)); // relative or absolute path?? TODO:  retrieve filepath from metadata
    blockMgr.include("xpcf/component/ConfigurableBase.h",false);
    blockMgr.include("xpcf/remoting/IGrpcService.h",false);
    blockMgr.include((*c)[ClassDescriptor::MetadataType::GRPCSERVICENAME] + ".grpc.pb.h");
    blockMgr.include("grpc/grpc.h",false);
    blockMgr.newline();

    blockMgr.out() << "namespace " + m_nameSpace;
    blockMgr.out() << " ";
    {
        block_guard<CPP::NSPACE> scopedNspace(blockMgr);
        blockMgr.newline();
        blockMgr.out() << "class " + m_className + ":  public org::bcom::xpcf::ConfigurableBase, virtual public org::bcom::xpcf::IGrpcService\n";
        {
            block_guard<CPP::CLASS> classBlk(blockMgr);
            {
                block_guard<CPP::PUBLIC> publicBlk(blockMgr);
                blockMgr.out() <<  m_className +"();\n";
                blockMgr.out() << "~"+ m_className +"() override = default;\n";
                blockMgr.out() << "::grpc::Service * getService() override;\n";
                blockMgr.out() << "void unloadComponent () override final;\n";
                blockMgr.out() << "org::bcom::xpcf::XPCFErrorCode onConfigured() override;\n";
                blockMgr.newline();

                blockMgr.out() << "class " + m_grpcClassName + "Impl:  public ::" +  c->getMetadata().at(ClassDescriptor::MetadataType::REMOTINGNSPACE) + "::" + m_grpcClassName+"::Service\n";
                {
                    block_guard<CPP::CLASS> innerClassBlk(blockMgr);
                    {
                        block_guard<CPP::PUBLIC> innerPublicBlk(blockMgr);
                        blockMgr.out() << m_grpcClassName + "Impl() = default;\n";// missing something to set the ref toward the injected I* component
                        // foreach method
                        processHeaderMethods(c,blockMgr);
                        std::string baseInterface;
                        if (!c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE).empty()) {
                            baseInterface = c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE) + "::";
                        }
                        baseInterface += c->getName();
                        blockMgr.out() << "SRef<" + baseInterface + "> m_xpcfComponent;\n";
                    }
                }
            }
            {
                block_guard<CPP::PRIVATE> privateBlk(blockMgr);
                blockMgr.out() << m_grpcClassName + "Impl m_grpcService;\n";
            }
        }
    }

    blockMgr.newline();

    blockMgr.out() << "template <> struct org::bcom::xpcf::ComponentTraits<" + m_nameSpace + "::" + m_className +">\n";//xpcf::grpc::proxy::c.name::c.name_grpcProxy>
    {
        block_guard<CPP::CLASS> classBlk(blockMgr);
        blockMgr.out() << "static constexpr const char * UUID = \"" + xpcf::uuids::to_string(serverUUID) + "\";\n";
        blockMgr.out() << "static constexpr const char * NAME = \"" + m_className + "\";\n";
        blockMgr.out() << "static constexpr const char * DESCRIPTION = \"" + m_className + " grpc server component\";\n";
    }
    blockMgr.includeGuardsEnd();
}

void ServerGenerator::bindInput(const ParameterDescriptor & p, CppBlockManager & blockMgr)
{
    std::stringstream requestParamTranscription;
    if (p.ioType() == ParameterDescriptor::io_type::in || p.ioType() == ParameterDescriptor::io_type::inout) {
        if (p.type().kind() == type_kind::builtin_t
                || p.type().kind() == type_kind::enum_t
                || p.type().kind() == type_kind::std_string_t) {
            if (p.type().needsStaticCast()) {
                requestParamTranscription << " = static_cast<" << p.type().getFullTypeDescription() << ">(request->" << boost::to_lower_copy(p.getName()) << "());\n";
            }
            else {
                requestParamTranscription << " = request->" << boost::to_lower_copy(p.getName()) << "();\n";
            }
        }
        else {
            requestParamTranscription << " = xpcf::deserialize<"<<p.type().getFullTypeDescription()<<">(request->" << boost::to_lower_copy(p.getName()) << "());\n";
        }
    }
    else {
        requestParamTranscription << ";\n";
    }

    blockMgr.out() << p.type().getFullTypeDescription() << " " << p.getName() << requestParamTranscription.str();
}

void ServerGenerator::bindOutput(const ParameterDescriptor & p, CppBlockManager & blockMgr)
{
    if (p.ioType() == ParameterDescriptor::io_type::inout || p.ioType() == ParameterDescriptor::io_type::out) {
        if (p.type().kind() == type_kind::builtin_t
                || p.type().kind() == type_kind::enum_t
                || p.type().kind() == type_kind::std_string_t) {
            if (p.type().needsStaticCast()) {
                blockMgr.out() << "response->set_"<< boost::to_lower_copy(p.getName()) <<"(static_cast<"<< p.type().getRPCType() <<">("<<p.getName() <<"));\n";
            }
            else {
                blockMgr.out() << "response->set_" << boost::to_lower_copy(p.getName()) << "(" << p.getName() << ");\n";
            }
        }
        else if (p.type().kind() == type_kind::user_defined_t) {
            blockMgr.out() << "response->set_" << boost::to_lower_copy(p.getName()) << "(xpcf::serialize<" << p.type().getFullTypeDescription() <<">(" << p.getName() << "));\n";
        }
    }
}

void ServerGenerator::processBodyMethods(const SRef<ClassDescriptor> c, CppBlockManager & blockMgr)
{
    for (auto m : c->methods()) {
        std::string request = m->m_requestName;
        std::string response = m->m_responseName;

        blockMgr.out() << "::grpc::Status " + m_className +"::"+ m_grpcClassName + "Impl::" + m->m_rpcName + "(::grpc::ServerContext* context, const "+ request + "* request, " + response + "* response)\n";
        {
            block_guard methodBlock(blockMgr);
            std::stringstream methodCall;
            if (!m->returnType().isVoid()) {
                if (m->returnType().kind() == type_kind::c_string_t) {
                    methodCall << "std::string" << " returnValue = ";
                }
                else {
                    methodCall << m->returnType().getFullTypeDescription()<< " returnValue = ";
                }
            }
            methodCall << "m_xpcfComponent->" << m->getName() << "(";
            uint32_t nbTypes = 0;
            for (SRef<ParameterDescriptor> p : m->m_params) {
                if (nbTypes != 0) {
                    methodCall << ", ";
                }
                bindInput(*p, blockMgr);
                methodCall << p->getName();
                nbTypes++;
            }
            blockMgr.out() << methodCall.str() << ");\n";
            for (SRef<ParameterDescriptor> p : m->m_params) {
                bindOutput(*p, blockMgr);
            }
            if (!m->returnType().isVoid()) {
                // TODO : serialize return type !!!
                if (m->returnType().kind() == type_kind::builtin_t) {
                    if (m->returnType().needsStaticCast()) {
                        blockMgr.out() << "response->set_xpcfgrpcreturnvalue(static_cast<"<< m->returnType().getRPCType() <<">(returnValue));\n";
                    }
                    else {
                        blockMgr.out() << "response->set_xpcfgrpcreturnvalue(returnValue);\n";
                    }
                }
                else if (m->returnType().kind() == type_kind::user_defined_t) {
                    blockMgr.out() << "response->set_xpcfgrpcreturnvalue(xpcf::serialize<" << m->returnType().getFullTypeDescription() << ">(returnValue));\n";
                }
                else if (m->returnType().kind() == type_kind::enum_t) {
                    if (m->returnType().needsStaticCast()) {
                        blockMgr.out() << "response->set_xpcfgrpcreturnvalue(static_cast<"<< m->returnType().getRPCType() <<">(returnValue));\n";
                    }
                    else {
                        blockMgr.out() << "response->set_xpcfgrpcreturnvalue(returnValue);\n";
                    }
                }
                else if (m->returnType().kind() == type_kind::c_string_t ) {
                    blockMgr.out() << "response->set_xpcfgrpcreturnvalue(returnValue);\n";
                }
            }
            blockMgr.out() << "return ::grpc::Status::OK;\n";
        }
        blockMgr.newline();
    }
}

void ServerGenerator::generateBody(const SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata, std::ostream& out)
{
    CppBlockManager blockMgr(out);
    out << "// GRPC Server Class implementation generated with xpcf_grpc_gen\n";
    out << "#include \"" + m_headerFileName + "\"\n";
    out << "#include <cstddef>\n";
    blockMgr.include("xpcf/remoting/ISerializable.h",false);

    // the body will use the grpcCLient generated from the proto or flat generators hence the following inclusion :
    //#include "grpcgeneratedclient.grpc.[pb|fb].h"


    blockMgr.out() << "namespace xpcf = org::bcom::xpcf;\n";
    blockMgr.newline();
    blockMgr.out() << "template<> " + m_nameSpace + "::" + m_className + "* xpcf::ComponentFactory::createInstance<"+ m_nameSpace + "::" + m_className + ">();\n";
    blockMgr.newline();

    blockMgr.out() << "namespace " + m_nameSpace;
    blockMgr.out() << " ";
    {
        block_guard<CPP::NSPACE> nspaceBlk(blockMgr);
        blockMgr.newline();
        blockMgr.out() << m_className + "::" + m_className + "():xpcf::ConfigurableBase(xpcf::toMap<"+ m_className + ">())\n";
        {
            block_guard methodBlock(blockMgr);
            blockMgr.out() << "declareInterface<xpcf::IGrpcService>(this);\n";
            std::string baseInterface;
            if (!c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE).empty()) {
                baseInterface = c->getMetadata().at(ClassDescriptor::MetadataType::INTERFACENAMESPACE) + "::";
            }
            baseInterface += c->getName();
            blockMgr.out() << "declareInjectable<" + baseInterface + ">(m_grpcService.m_xpcfComponent);\n";
        }
        blockMgr.newline();
        blockMgr.out() << "void " + m_className + "::unloadComponent ()\n";
        {
            block_guard methodBlock(blockMgr);
            blockMgr.out() << "delete this;\n";
            blockMgr.out() << "return;\n";
        }
        blockMgr.newline();
        blockMgr.out() << "XPCFErrorCode " + m_className +"::onConfigured()\n";
        {
            block_guard methodBlock(blockMgr);
        }
        blockMgr.newline();
        blockMgr.out() << "::grpc::Service * " + m_className +"::getService()\n";
        {
            block_guard methodBlock(blockMgr);
            blockMgr.out() << "return &m_grpcService;\n";
        }
        processBodyMethods(c, blockMgr);
    }
}

std::map<IRPCGenerator::MetadataType,std::string> ServerGenerator::generateImpl(SRef<ClassDescriptor> c, std::map<MetadataType,std::string> metadata)
{
    m_nameSpace =  "org::bcom::xpcf::grpc::server" + c->getName();
    m_className = c->getName() + "_grpcServer";
    m_headerFileName = m_className + ".h";
    m_cppFileName = m_className + ".cpp";
    m_grpcClassName = (*c)[ClassDescriptor::MetadataType::GRPCSERVICENAME];
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
    metadata[SERVER_XPCFGRPCCOMPONENTNAME] = m_className;
    metadata[SERVER_XPCFGRPCNAMESPACE] = m_nameSpace;
    metadata[MetadataType::SERVER_HEADERFILENAME] = m_headerFileName;
    metadata[MetadataType::SERVER_CPPFILENAME] = m_cppFileName;
    return metadata;
}

void ServerGenerator::finalizeImpl(std::map<MetadataType,std::string> metadata)
{
    for (auto [name, uuid] : m_serviceUuidMap) {
        std::cout << "Component " << name << " generated uuid = [[xpcf::serverUUID(" << uuid <<")]]" << std::endl;
    }
    m_serviceUuidMap.clear();
}

