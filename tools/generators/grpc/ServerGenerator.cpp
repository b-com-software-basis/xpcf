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

void ServerGenerator::generateHeader(const ClassDescriptor & c, std::ostream& out)
{
    //NOTE : proxy is configurable to set grpc channel etc...
    xpcf::uuids::random_generator gen;
    xpcf::uuids::uuid proxyUUID = gen();
    CppBlockManager blockMgr(out);
    blockMgr.out() << "// GRPC Server Class Header generated with xpcf_grpc_gen\n";
    blockMgr.newline();
    blockMgr.includeGuardsStart(m_className);
    blockMgr.out()<< "#include \"" + c.getName() +".h\"\n"; // relative or absolute path?? TODO:  retrieve filepath from metadata
    blockMgr.out() << "#include <xpcf/component/ConfigurableBase.h>\n";
    blockMgr.out() << "#include <xpcf/remoting/IGrpcService.h>\n";
    blockMgr.newline();

    blockMgr.out() << "namespace " + m_nameSpace;
    blockMgr.out() << " ";
    {
        block_guard<CPP::NSPACE> scopedNspace(blockMgr);
        blockMgr.out() << "class " + m_className + ":  public org::bcom::xpcf::ConfigurableBase, virtual public org::bcom::xpcf::IGrpcService\n";
        {
            block_guard<CPP::CLASS> classBlk(blockMgr);
            {
                block_guard<CPP::PUBLIC> publicBlk(blockMgr);
                blockMgr.out() <<  m_className +"();\n";
                blockMgr.out() << "~"+ m_className +"() override = default;\n";
                blockMgr.out() << "grpc::Service * getService() override;\n";
                blockMgr.out() << "void unloadComponent () override final;\n";
                blockMgr.out() << "XPCFErrorCode onConfigured() override;\n";
                blockMgr.newline();

                blockMgr.out() << "class " + m_grpcClassName + "Impl:  public "+ m_grpcClassName+"::Service\n";
                {
                    block_guard<CPP::CLASS> innerClassBlk(blockMgr);
                    {
                        block_guard<CPP::PUBLIC> innerPublicBlk(blockMgr);
                        blockMgr.out() << m_grpcClassName + "Impl() = default;\n";// missing something to set the ref toward the injected I* component
                        // foreach method
                        for (auto m : c.methods()) {
                            blockMgr.out() << "Status " + m.m_rpcName + "(ServerContext* context, " + m.m_requestName + ", " + m.m_responseName + ") override;\n";
                        }
                        blockMgr.newline();
                        blockMgr.out() << "SRef<" + c.getName() + "> m_xpcfComponent;\n";
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
        blockMgr.out() << "static constexpr const char * UUID = \"" + xpcf::uuids::to_string(proxyUUID) + "\";\n";
        blockMgr.out() << "static constexpr const char * NAME = \"" + m_className + "\";\n";
        blockMgr.out() << "static constexpr const char * DESCRIPTION = \"" + m_className + " grpc server component\";\n";
    }
    blockMgr.includeGuardsEnd();
}

void ServerGenerator::generateBody(const ClassDescriptor & c, std::ostream& out)
{
    CppBlockManager blockMgr(out);
    out << "// GRPC Server Class implementation generated with xpcf_grpc_gen\n";
    out << "#include \"" + m_headerFileName + "\"\n";
    out << "#include <cstddef>\n";
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
        blockMgr.out() << m_className + "::" + m_className + "()\n";
        {
            block_guard methodBlock(blockMgr);
            blockMgr.out() << "declareInterface<xpcf::IGrpcService>(this);\n";
            blockMgr.out() << "declareInjectable<" +  c.getName() + ">(m_grpcService.m_xpcfComponent);\n";
        }
        blockMgr.newline();
        out << "void " + m_className + "::unloadComponent ()\n";
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
        blockMgr.out() << "grpc::Service * " + m_className +"::getService()\n";
        {
            block_guard methodBlock(blockMgr);
            blockMgr.out() << "return &m_grpcService;\n";
        }

        for (auto m : c.methods()) {
            out << "Status "+ m_grpcClassName + "Impl()::" + m.m_rpcName + "(ServerContext* context,"+ m.m_requestName+", "+m.m_responseName+")\n";
            {
                block_guard methodBlock(blockMgr);
            }
        }
    }
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
        generateHeader(c, headerFile);
        headerFile.close();
        std::ofstream cppFile(cppFilePath.generic_string(utf8).c_str(), std::ios::out);
        generateBody(c, cppFile);
        cppFile.close();
    }
    metadata[SERVER_XPCFGRPCCOMPONENTNAME] = m_className;
    metadata[SERVER_XPCFGRPCNAMESPACE] = m_nameSpace;
    metadata[MetadataType::SERVER_HEADERFILENAME] = m_headerFileName;
    metadata[MetadataType::SERVER_CPPFILENAME] = m_cppFileName;
    return metadata;
}
