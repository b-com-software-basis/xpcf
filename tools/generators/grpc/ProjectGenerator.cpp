#include "ProjectGenerator.h"

namespace xpcf = org::bcom::xpcf;

template<> ProjectGenerator * xpcf::ComponentFactory::createInstance<ProjectGenerator>();



ProjectGenerator::ProjectGenerator():AbstractGenerator(xpcf::toMap<ProjectGenerator>())
{
    m_headerProjectInfos << "\nHEADERS +=";
    m_srcProjectInfos << "\nSOURCES +=";
    m_protoProjectInfos << "\nPROTO =";
    m_moduleHdrMgr = std::make_unique<CppBlockManager>(m_moduleMainInfosHeader);
    m_moduleSrcMgr = std::make_unique<CppBlockManager>(m_moduleMainInfosSource);
    m_moduleHdrMgr->out() << "// GRPC Proxy Class Header generated with xpcf_grpc_gen\n\n";
    m_moduleHdrMgr->newline();
    m_moduleHdrMgr->includeGuardsStart("xpcfGrpcModuleMain");
    m_moduleSrcMgr->include("xpcfGrpcModuleMain.h");
    m_moduleSrcMgr->include("xpcf/module/ModuleFactory.h", false);
    m_moduleSrcMgr->out() << "namespace xpcf=org::bcom::xpcf;";
    // need project/fwk name to mangle module name ?
    m_moduleSrcMgr->out() << "XPCF_DECLARE_MODULE(\"\", \"xpcfGrpcModule\",\"xpcfGrpcModule\");\n";
    m_moduleSrcMgr->newline();
    m_moduleSrcMgr->out() << "extern \"C\" XPCF_MODULEHOOKS_API xpcf::XPCFErrorCode XPCF_getComponent(const boost::uuids::uuid& componentUUID,SRef<xpcf::IComponentIntrospect>& interfaceRef)\n";
    {
        m_moduleSrcMgr->enter();
        m_moduleSrcMgr->out() << "xpcf::XPCFErrorCode errCode = xpcf::XPCFErrorCode::_FAIL;\n";
    }
    m_moduleMainDeclareComponents << "XPCF_BEGIN_COMPONENTS_DECLARATION\n";
}


ProjectGenerator::~ProjectGenerator()
{

}

void ProjectGenerator::generateModuleMain(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata)
{
    m_moduleHdrMgr->include(metadata[MetadataType::PROXY_HEADERFILENAME]);
    m_moduleHdrMgr->include(metadata[MetadataType::SERVER_HEADERFILENAME]);

    std::string fullComponentName = metadata[MetadataType::PROXY_XPCFGRPCNAMESPACE] + "::" + metadata[MetadataType::PROXY_XPCFGRPCCOMPONENTNAME];

    m_moduleSrcMgr->out()<<"if (errCode != xpcf::XPCFErrorCode::_SUCCESS) {\n";
    {
        block_guard condBlk(*m_moduleSrcMgr);
        m_moduleSrcMgr->out()<<"errCode = xpcf::tryCreateComponent<" + fullComponentName + ">(componentUUID,interfaceRef);\n";
    }
    m_moduleMainDeclareComponents<<"XPCF_ADD_COMPONENT(" <<fullComponentName <<")\n";

}

std::map<IRPCGenerator::MetadataType,std::string> ProjectGenerator::generate(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata)
{
    m_headerProjectInfos << "\\\n" << metadata.at(MetadataType::PROXY_HEADERFILENAME);
    m_headerProjectInfos << "\\\n" << metadata.at(MetadataType::SERVER_HEADERFILENAME);
    m_srcProjectInfos << "\\\n" << metadata.at(MetadataType::PROXY_CPPFILENAME);
    m_srcProjectInfos << "\\\n" << metadata.at(MetadataType::SERVER_CPPFILENAME);
    m_protoProjectInfos << "\\\n" << metadata.at(MetadataType::GRPCPROTOFILENAME);
    generateModuleMain(c,metadata);
    return metadata;
}

void ProjectGenerator::finalize(std::map<MetadataType,std::string> metadata)
{
    m_moduleHdrMgr->includeGuardsEnd();
    m_moduleSrcMgr->out()<<"return errCode;\n";
    m_moduleSrcMgr->leave();
    m_moduleSrcMgr->newline();
    m_moduleMainDeclareComponents << "XPCF_END_COMPONENTS_DECLARATION\n";
    m_moduleSrcMgr->out() << m_moduleMainDeclareComponents.str();
    if (m_mode == GenerateMode::STD_COUT) {
        std::cout << m_protoProjectInfos.str();
        std::cout << m_headerProjectInfos.str();
        std::cout << m_srcProjectInfos.str();
        std::cout << m_moduleMainInfosHeader.str();
        std::cout << m_moduleMainInfosSource.str();
    }
    else {
        fs::detail::utf8_codecvt_facet utf8;
        fs::path projectFilePath("xpcfGrpcRemoting.pri",utf8);
        projectFilePath = m_folder/projectFilePath;
        std::ofstream projectFile(projectFilePath.generic_string(utf8).c_str(), std::ios::out);
        projectFile << m_protoProjectInfos.str();
        projectFile << m_headerProjectInfos.str();
        projectFile << m_srcProjectInfos.str();
        projectFile.close();
        fs::path moduleHeaderFilePath("xpcfGrpcModuleMain.h",utf8);
        moduleHeaderFilePath = m_folder/moduleHeaderFilePath;
        std::ofstream moduleHeaderFile(moduleHeaderFilePath.generic_string(utf8).c_str(), std::ios::out);
        moduleHeaderFile << m_moduleMainInfosHeader.str();
        moduleHeaderFile.close();
        fs::path moduleSourceFilePath("xpcfGrpcModuleMain.cpp",utf8);
        moduleSourceFilePath = m_folder/moduleSourceFilePath;
        std::ofstream moduleSourceFile(moduleSourceFilePath.generic_string(utf8).c_str(), std::ios::out);
        moduleSourceFile << m_moduleMainInfosSource.str();
        moduleSourceFile.close();
    }
}


