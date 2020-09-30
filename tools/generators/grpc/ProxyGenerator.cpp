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
    CppBlockManager blockMgr(out);
    //NOTE : proxy is configurable to set grpc channel etc...
    xpcf::uuids::random_generator gen;
    xpcf::uuids::uuid proxyUUID = gen();

    blockMgr.out() << "// GRPC Proxy Class Header generated with xpcf_grpc_gen\n\n";
    newline(out);
    includeGuardsStart(m_className, out);
    out<< "#include \"" + c.getName() +".h\"\n"; // relative or absolute path?? TODO:  retrieve filepath from metadata
    blockMgr.out() << "#include <xpcf/component/ConfigurableBase.h>\n";
    blockMgr.out() << "#include <memory>\n";
    blockMgr.out() << "#include <string>\n";
    newline(out);
    blockMgr.out() << "namespace " + m_nameSpace +" {\n";
    {
        auto nspaceBlk = blockMgr.enter<CPP::NSPACE>();
        blockMgr.out() << "class " + m_className + ":  public org::bcom::xpcf::ConfigurableBase, virtual public " + c.getName() + " {\n";
        {
            auto classBlk = blockMgr.enter<CPP::CLASS>();
            {
                auto publicBlk = blockMgr.enter<CPP::PUBLIC>();

                blockMgr.out() << m_className +"();\n";
                blockMgr.out() << "~"+ m_className +"() override = default;\n";
                blockMgr.out() << "void unloadComponent () override final;\n";
                blockMgr.out() << "XPCFErrorCode onConfigured() override;\n";
                newline(out);
                // foreach method
                for (auto m : c.methods()) {
                    blockMgr.out() <<  m.getFullDeclaration() + " override;\n";
                }
                newline(out);
            }
            {
                auto privateBlk = blockMgr.enter<CPP::PRIVATE>();
                blockMgr.out() << "std::string m_channelUrl;\n";
                blockMgr.out() << "uint32_t m_channelCredentials;\n";
                blockMgr.out() << "std::shared_ptr<grpc::Channel> m_channel;\n";
                blockMgr.out() << "std::unique_ptr<" + m_grpcClassName + "::Stub> m_grpcStub;\n";
            }
        }
    }
    newline(out);

    blockMgr.out() << "template <> struct org::bcom::xpcf::ComponentTraits<" + m_nameSpace + "::" + m_className +">\n";//xpcf::grpc::proxy::c.name::c.name_grpcProxy>
    {
        auto blk = blockMgr.enter<CPP::CLASS>();
        blockMgr.out() << "static constexpr const char * UUID = \"" + xpcf::uuids::to_string(proxyUUID) + "\";\n";
        blockMgr.out() << "static constexpr const char * NAME = \"" + m_className + "\";\n";
        blockMgr.out() << "static constexpr const char * DESCRIPTION = \"" + m_className + " grpc client proxy component\";\n";
    }
    newline(out);
    includeGuardsEnd(out);
}

void ProxyGenerator::generateBody(const ClassDescriptor & c, std::ostream& out)
{
    CppBlockManager blockMgr(out);
    blockMgr.out() << "// GRPC Proxy Class implementation generated with xpcf_grpc_gen\n";
    blockMgr.out() << "#include \"" + m_headerFileName + "\"\n";
    blockMgr.out() << "#include <cstddef>\n";
    // the body will use the grpcCLient generated from the proto or flat generators hence the following inclusion :
    //#include "grpcgeneratedclient.grpc.[pb|fb].h"
    blockMgr.out() << "namespace xpcf = org::bcom::xpcf;\n";


    newline(out);

    blockMgr.out() << "template<> " + m_nameSpace + "::" + m_className + "* xpcf::ComponentFactory::createInstance<"+ m_nameSpace + "::" + m_className + ">();\n";
    newline(out);
    blockMgr.out() << "namespace " + m_nameSpace + cppBeginBlock;
    {
        auto nspaceBlk = blockMgr.enter<CPP::NSPACE>();
        newline(out);
        blockMgr.out() << "typedef enum ";
        {
            auto enumBlk = blockMgr.enter();
            blockMgr.out() << "GoogleDefaultCredentials = 0,\n";
            blockMgr.out() << "SslCredentials = 1,\n";
            blockMgr.out() << "InsecureChannelCredentials = 2\n";
        }
        blockMgr.out() << " grpcCredentials;\n";

        blockMgr.out() << "inline std::shared_ptr< grpc::ChannelCredentials > getCredentials(grpcCredentials channelCredentials)\n";
        {
            auto methodBlk = blockMgr.enter();
            blockMgr.out() << "switch (channelCredentials) {\n";
            blockMgr.out() << "case GoogleDefaultCredentials:\n";
            blockMgr.out() << "return grpc::GoogleDefaultCredentials();\n";
            blockMgr.out() << "case SslCredentials:\n";
            blockMgr.out() << "return grpc::SslCredentials(grpc::SslCredentialsOptions());\n";
            blockMgr.out() << "case InsecureChannelCredentials:       \n";
            blockMgr.out() << "return grpc::InsecureChannelCredentials();\n";
            blockMgr.out() << "default:   \n";
            blockMgr.out() << "return nullptr;\n";
            blockMgr.out() << "}\n";
            blockMgr.out() << "return nullptr;\n";
        }
        newline(out);
        blockMgr.out() << m_className + "::" + m_className + "():xpcf::ConfigurableBase(xpcf::toMap<"+ m_className + ">())\n";
        {
            auto methodBlk = blockMgr.enter();
            blockMgr.out() << "declareInterface<" +  c.getName() + ">(this);\n";
            blockMgr.out() << "declareProperty(\"channelUrl\",m_channelUrl);\n";
            blockMgr.out() << "declareProperty(\"channelCredentials\",m_channelCredentials);\n";
        }
        newline(out);
        blockMgr.out() << "void " + m_className + "::unloadComponent ()\n";
        {
            auto methodBlk = blockMgr.enter();
            blockMgr.out() << "delete this;\n";
            blockMgr.out() << "return;\n";
        }
        newline(out);
        blockMgr.out() << "XPCFErrorCode " + m_className +"::onConfigured()\n";
        {
            auto methodBlk = blockMgr.enter();
            blockMgr.out() << "m_channel = grpc::CreateChannel(m_channelUrl, getCredentials(m_channelCredentials));\n";
            blockMgr.out() << "m_grpcStub =" + m_grpcClassName + "::NewStub(m_channel);\n";
        }
        newline(out);

        for (auto m : c.methods()) {
            blockMgr.out() << m.getReturnType() + "  "+ m_className + "::" + m.getDeclaration() + "\n";
            {
                auto methodBlk = blockMgr.enter();//missing serialize/deserialize from method cpp params !
                blockMgr.out() << "ClientContext context;\n";
                blockMgr.out() << "Status status = m_grpcStub->" + m.m_rpcName + "(&context, " + m.m_requestName + "," + m.m_responseName + ");\n";
                blockMgr.out() << "if (!status.ok())";
                {
                    auto condBlk = blockMgr.enter();
                    blockMgr.out() << "std::cblockMgr.out() << \"" + m.m_rpcName + "rpc failed.\" << std::endl;\n";
                    blockMgr.out() << "return static_cast<" + m.getReturnType() + ">(XPCFErrorCode::_FAIL);\n";//TODO : differentiate semantic return type from status return type : provide status type name ?
                }
                blockMgr.out() << " return static_cast<" + m.getReturnType() + ">(XPCFErrorCode::_SUCCESS);\n";
            }
            newline(out);
        }
    }
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
    metadata[MetadataType::HEADERFILENAME] = m_headerFileName;
    metadata[MetadataType::CPPFILENAME] = m_cppFileName;
    return metadata;
}

