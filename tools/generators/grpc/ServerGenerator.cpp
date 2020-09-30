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
    CppBlockManager m_block(out);
    m_block.out() << "// GRPC Server Class Header generated with xpcf_grpc_gen\n";
    m_block.newline();
    includeGuardsStart(m_className, out);
    m_block.out()<< "#include \"" + c.getName() +".h\"\n"; // relative or absolute path?? TODO:  retrieve filepath from metadata
    m_block.out() << "#include <xpcf/component/ConfigurableBase.h>\n";
    m_block.out() << "#include <xpcf/remoting/IGrpcService.h>\n";
    m_block.newline();

    m_block.out() << "namespace " + m_nameSpace;
    m_block.out() << " ";
    {
        auto scopedNspace = m_block.enter<CPP::NSPACE>();
        m_block.out() << "class " + m_className + ":  public org::bcom::xpcf::ConfigurableBase, virtual public org::bcom::xpcf::IGrpcService\n";
        {
            auto classBlk = m_block.enter<CPP::CLASS>();
            {
                auto publicBlk = m_block.enter<CPP::PUBLIC>();
                m_block.out() <<  m_className +"();\n";
                m_block.out() << "~"+ m_className +"() override = default;\n";
                m_block.out() << "grpc::Service * getService() override;\n";
                m_block.out() << "void unloadComponent () override final;\n";
                m_block.out() << "XPCFErrorCode onConfigured() override;\n";
                m_block.newline();

                m_block.out() << "class " + m_grpcClassName + "Impl:  public "+ m_grpcClassName+"::Service\n";
                {
                    auto innerClassBlk = m_block.enter<CPP::CLASS>();
                    {
                        auto innerPublicBlk = m_block.enter<CPP::PUBLIC>();
                        m_block.out() << m_grpcClassName + "Impl() = default;\n";// missing something to set the ref toward the injected I* component
                        // foreach method
                        for (auto m : c.methods()) {
                            m_block.out() << "Status " + m.m_rpcName + "(ServerContext* context, " + m.m_requestName + ", " + m.m_responseName + ") override;\n";
                        }
                        m_block.newline();
                        m_block.out() << "SRef<" + c.getName() + "> m_xpcfComponent;\n";
                    }
                }
            }
            {
                auto privateBlk = m_block.enter<CPP::PRIVATE>();
                m_block.out() << m_grpcClassName + "Impl m_grpcService;\n";
            }
        }
    }

    m_block.newline();

    m_block.out() << "template <> struct org::bcom::xpcf::ComponentTraits<" + m_nameSpace + "::" + m_className +">\n";//xpcf::grpc::proxy::c.name::c.name_grpcProxy>
    {
        auto blk = m_block.enter();
        m_block.out() << "static constexpr const char * UUID = \"" + xpcf::uuids::to_string(proxyUUID) + "\";\n";
        m_block.out() << "static constexpr const char * NAME = \"" + m_className + "\";\n";
        m_block.out() << "static constexpr const char * DESCRIPTION = \"" + m_className + " grpc server component\";\n";
    }
    includeGuardsEnd(out);
}

void ServerGenerator::generateBody(const ClassDescriptor & c, std::ostream& out)
{
    CppBlockManager m_block(out);
    out << "// GRPC Server Class implementation generated with xpcf_grpc_gen\n";
    out << "#include \"" + m_headerFileName + "\"\n";
    out << "#include <cstddef>\n";
    // the body will use the grpcCLient generated from the proto or flat generators hence the following inclusion :
    //#include "grpcgeneratedclient.grpc.[pb|fb].h"


    m_block.out() << "namespace xpcf = org::bcom::xpcf;\n";
    m_block.newline();
    m_block.out() << "template<> " + m_nameSpace + "::" + m_className + "* xpcf::ComponentFactory::createInstance<"+ m_nameSpace + "::" + m_className + ">();\n";
    m_block.newline();

    m_block.out() << "namespace " + m_nameSpace;
    m_block.out() << " ";
    {
        auto blk = m_block.enter<CPP::NSPACE>();
        m_block.newline();
        m_block.out() << m_className + "::" + m_className + "()\n";
        {
            auto methodBlock = m_block.enter();
            m_block.out() << "declareInterface<xpcf::IGrpcService>(this);\n";
            m_block.out() << "declareInjectable<" +  c.getName() + ">(m_grpcService.m_xpcfComponent);\n";
        }
        m_block.newline();
        out << "void " + m_className + "::unloadComponent ()\n";
        {
            auto methodBlock = m_block.enter();
            m_block.out() << "delete this;\n";
            m_block.out() << "return;\n";
        }
        m_block.newline();
        m_block.out() << "XPCFErrorCode " + m_className +"::onConfigured()\n";
        {
            auto methodBlock = m_block.enter();
        }
        m_block.newline();
        m_block.out() << "grpc::Service * " + m_className +"::getService()\n";
        {
            auto methodBlock = m_block.enter();
            m_block.out() << "return &m_grpcService;\n";
        }

        for (auto m : c.methods()) {
            out << "Status "+ m_grpcClassName + "Impl()::" + m.m_rpcName + "(ServerContext* context,"+ m.m_requestName+", "+m.m_responseName+")\n";
            {
                auto methodBlock = m_block.enter();
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
        std::ofstream cppFile(cppFilePath.generic_string(utf8).c_str(), std::ios::out);
        generateHeader(c, headerFile);
        generateBody(c, cppFile);
    }
    return metadata;
}
