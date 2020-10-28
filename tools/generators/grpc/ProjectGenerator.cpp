#include "ProjectGenerator.h"

namespace xpcf = org::bcom::xpcf;

template<> ProjectGenerator * xpcf::ComponentFactory::createInstance<ProjectGenerator>();

std::string findRemakenRulesStr = "# Author(s) : Loic Touraine, Stephane Leduc\n\nandroid {\n    # unix path\nUSERHOMEFOLDER = $$clean_path($$(HOME))\n"
"    isEmpty(USERHOMEFOLDER) {\n       # windows path\n        USERHOMEFOLDER = $$clean_path($$(USERPROFILE))\n        isEmpty(USERHOMEFOLDER) {\n"
"            USERHOMEFOLDER = $$clean_path($$(HOMEDRIVE)$$(HOMEPATH))\n        }\n    }\n}\n\n"
"unix:!android {\n    USERHOMEFOLDER = $$clean_path($$(HOME))\n}\n\n"
"win32 {\n    USERHOMEFOLDER = $$clean_path($$(USERPROFILE))\n    isEmpty(USERHOMEFOLDER) {\n"
"        USERHOMEFOLDER = $$clean_path($$(HOMEDRIVE)$$(HOMEPATH))\n    }\n}\n\n"
"exists(builddefs/qmake) {\n    QMAKE_REMAKEN_RULES_ROOT=builddefs/qmake\n}\n"
"else {\n    QMAKE_REMAKEN_RULES_ROOT = $$clean_path($$(REMAKEN_RULES_ROOT))\n    !isEmpty(QMAKE_REMAKEN_RULES_ROOT) {\n"
"        QMAKE_REMAKEN_RULES_ROOT = $$clean_path($$(REMAKEN_RULES_ROOT)/qmake)\n    }\n"
"    else {\n        QMAKE_REMAKEN_RULES_ROOT=$${USERHOMEFOLDER}/.remaken/rules/qmake\n"
"    }\n}\n\n"
"!exists($${QMAKE_REMAKEN_RULES_ROOT}) {\n"
"    error(\"Unable to locate remaken rules in \" $${QMAKE_REMAKEN_RULES_ROOT} \". Either check your remaken installation, or provide the path to your remaken qmake root folder rules in REMAKEN_RULES_ROOT environment variable.\")\n"
"}\n\nmessage(\"Remaken qmake build rules used : \" $$QMAKE_REMAKEN_RULES_ROOT)\n";

// TODO : package information should depend upon the generator used ! and grpc/protobuf version should be added from metadata !
std::string pkgdepsStr = "grpc|1.32.0|grpc++|brew@system|\n"
"grpc|1.32.0|grpc++|apt-get@system|\n"
"protobuf|3.13.0|protobuf|brew@system|\n"
"protobuf|3.13.0|protobuf|apt-get@system|\n"
"xpcf|2.5.0|xpcf|@github|https://github.com/SolarFramework/binaries/releases/download\n";

ProjectGenerator::ProjectGenerator():AbstractGenerator(xpcf::toMap<ProjectGenerator>())
{
    m_headerProjectInfos << "\nHEADERS +=";
    m_srcProjectInfos << "\nSOURCES +=";
    m_protoProjectInfos << "\nPROTO =";
    m_moduleHdrMgr = std::make_unique<CppBlockManager>(m_moduleMainInfosHeader);
    m_moduleSrcMgr = std::make_unique<CppBlockManager>(m_moduleMainInfosSource);
    m_moduleDeclareMgr = std::make_unique<CppBlockManager>(m_moduleMainDeclareModule);
    m_moduleHdrMgr->out() << "// GRPC Proxy Class Header generated with xpcf_grpc_gen\n\n";
    m_moduleHdrMgr->newline();
    m_moduleHdrMgr->includeGuardsStart("xpcfGrpcModuleMain");
    m_moduleDeclareMgr->include("xpcfGrpcModuleMain.h");
    m_moduleDeclareMgr->include("xpcf/module/ModuleFactory.h", false);
    m_moduleDeclareMgr->out() << "namespace xpcf=org::bcom::xpcf;\n";
    // need project/fwk name to mangle module name ?
    //NOTE : proxy is configurable to set grpc channel etc...
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

    m_moduleSrcMgr->out()<<"if (errCode != xpcf::XPCFErrorCode::_SUCCESS) ";
    {
        block_guard condBlk(*m_moduleSrcMgr);
        m_moduleSrcMgr->newline();
        m_moduleSrcMgr->out()<<"errCode = xpcf::tryCreateComponent<" + fullComponentName + ">(componentUUID,interfaceRef);\n";
    }

    fullComponentName = metadata[MetadataType::SERVER_XPCFGRPCNAMESPACE] + "::" + metadata[MetadataType::SERVER_XPCFGRPCCOMPONENTNAME];

    m_moduleSrcMgr->out()<<"if (errCode != xpcf::XPCFErrorCode::_SUCCESS) ";
    {
        block_guard condBlk(*m_moduleSrcMgr);
        m_moduleSrcMgr->newline();
        m_moduleSrcMgr->out()<<"errCode = xpcf::tryCreateComponent<" + fullComponentName + ">(componentUUID,interfaceRef);\n";
    }
    m_moduleMainDeclareComponents<<"XPCF_ADD_COMPONENT(" <<fullComponentName <<")\n";

}

void ProjectGenerator::generateProjectFile(std::map<MetadataType,std::string> metadata, std::ostream& out)
{
    out << "## remove Qt dependencies\nQT       -= core gui\nCONFIG -= qt\n";

    out << "\n## global definitions : target lib name, version\n";
    out << "TARGET = xpcfGrpcRemoting"<< metadata[MetadataType::PROJECT_NAME] << "\n";
    out << "FRAMEWORK = $$TARGET\n";
    out << "VERSION="<< metadata[MetadataType::PROJECT_VERSION] << "\n";
    out << "\nDEFINES += MYVERSION=$${VERSION}\nDEFINES += TEMPLATE_LIBRARY\nCONFIG += c++1z\n\n";
    out << "include(findremakenrules.pri)\n\n";
    out << "CONFIG(debug,debug|release) {\n    DEFINES += _DEBUG=1\n    DEFINES += DEBUG=1\n}\n";
    out << "CONFIG(release,debug|release) {\n    DEFINES += _NDEBUG=1\n    DEFINES += NDEBUG=1\n}\n\n";
    out << "DEPENDENCIESCONFIG = shared recursive install_recurse\n\n";
    out << "## Configuration for Visual Studio to install binaries and dependencies. Work also for QT Creator by replacing QMAKE_INSTALL\n";
    out << "PROJECTCONFIG = QTVS\n\n";
    out << "#NOTE : CONFIG as staticlib or sharedlib, DEPENDENCIESCONFIG as staticlib or sharedlib, QMAKE_TARGET.arch and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibconfig.pri inclusion\n";
    out << "include ($${QMAKE_REMAKEN_RULES_ROOT}/templatelibconfig.pri)\n\n";
    out << m_headerProjectInfos.str();
    out << m_srcProjectInfos.str();
   // out << "include (xpcfGrpcRemoting" << metadata[MetadataType::PROJECT_NAME] << ".pri\n\n";
    out << "\nunix:!android {\n    QMAKE_CXXFLAGS += -Wignored-qualifiers\n#    QMAKE_LINK=clang++\n#    QMAKE_CXX = clang++\n}\n\n";
    out << "macx {\n    DEFINES += _MACOS_TARGET_\n    QMAKE_MAC_SDK= macosx\n";
    out << "    QMAKE_CFLAGS += -mmacosx-version-min=10.7 -std=c11 #-x objective-c++\n";
    out << "    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=c11 -std=c++11 -O3 -fPIC#-x objective-c++\n";
    out << "    QMAKE_LFLAGS += -mmacosx-version-min=10.7 -v -lstdc++\n";
    out << "    LIBS += -lstdc++ -lc -lpthread\n";
    out << "    LIBS += -L/usr/local/lib\n}\n\n";
    out << "win32 {\n\n    DEFINES += WIN64 UNICODE _UNICODE\n    QMAKE_COMPILER_DEFINES += _WIN64\n";
    out << "    QMAKE_CXXFLAGS += -wd4250 -wd4251 -wd4244 -wd4275 /Od\n}\n\n";
    out << "android {\n    QMAKE_LFLAGS += -nostdlib++\n}\n\n";
    out << "OTHER_FILES += \\n    packagedependencies.txt\n\n";
    out << "#NOTE : Must be placed at the end of the .pro\ninclude ($${QMAKE_REMAKEN_RULES_ROOT}/remaken_install_target.pri)\n";

}

std::map<IRPCGenerator::MetadataType,std::string> ProjectGenerator::generate(ClassDescriptor & c, std::map<MetadataType,std::string> metadata)
{
    m_headerProjectInfos << " \\\n" << metadata.at(MetadataType::PROXY_HEADERFILENAME);
    m_headerProjectInfos << " \\\n" << metadata.at(MetadataType::SERVER_HEADERFILENAME);
    m_headerProjectInfos << " \\\n" << metadata[MetadataType::GRPCSERVICENAME] << ".pb.h";
    m_headerProjectInfos << " \\\n" << metadata[MetadataType::GRPCSERVICENAME] << ".grpc.pb.h";
    m_srcProjectInfos << " \\\n" << metadata.at(MetadataType::PROXY_CPPFILENAME);
    m_srcProjectInfos << " \\\n" << metadata.at(MetadataType::SERVER_CPPFILENAME);
    m_srcProjectInfos << " \\\n" << metadata[MetadataType::GRPCSERVICENAME] << ".pb.cc";
    m_srcProjectInfos << " \\\n" << metadata[MetadataType::GRPCSERVICENAME] << ".grpc.pb.cc";
    m_protoProjectInfos << " \\\n" << metadata.at(MetadataType::GRPCPROTOFILENAME);
    generateModuleMain(c,metadata);
    return metadata;
}

void ProjectGenerator::finalize(std::map<MetadataType,std::string> metadata)
{
    xpcf::uuids::random_generator gen;
    xpcf::uuids::uuid moduleUUID = gen();
    m_moduleDeclareMgr->out() << "XPCF_DECLARE_MODULE(\""<< xpcf::uuids::to_string(moduleUUID) <<"\", \"xpcfGrpcModule\",\"xpcfGrpcModule for project "<< metadata[MetadataType::PROJECT_NAME] <<"\");\n";
    m_moduleHdrMgr->includeGuardsEnd();
    m_moduleSrcMgr->out()<<"return errCode;\n";
    m_moduleSrcMgr->leave();
    m_moduleSrcMgr->newline();
    m_moduleMainDeclareComponents << "XPCF_END_COMPONENTS_DECLARATION\n";
    m_moduleSrcMgr->out() << m_moduleMainDeclareComponents.str();
    m_headerProjectInfos << " \\\n" << "xpcfGrpcModuleMain.h" << "\n";
    m_srcProjectInfos << " \\\n" << "xpcfGrpcModuleMain.cpp" << "\n";

    if (m_mode == GenerateMode::STD_COUT) {
        generateProjectFile(metadata,std::cout);
        std::cout << m_protoProjectInfos.str();
        std::cout << m_headerProjectInfos.str();
        std::cout << m_srcProjectInfos.str();
        std::cout << m_moduleMainInfosHeader.str();
        std::cout << m_moduleMainDeclareModule.str();
    }
    else {
        fs::detail::utf8_codecvt_facet utf8;
        fs::path projectFilePath("xpcfGrpcRemoting" + metadata[MetadataType::PROJECT_NAME] + ".pro",utf8);
        projectFilePath = m_folder/projectFilePath;
        std::ofstream projectFile(projectFilePath.generic_string(utf8).c_str(), std::ios::out);
        generateProjectFile(metadata,projectFile);
        projectFile.close();
        fs::path moduleHeaderFilePath("xpcfGrpcModuleMain.h",utf8);
        moduleHeaderFilePath = m_folder/moduleHeaderFilePath;
        std::ofstream moduleHeaderFile(moduleHeaderFilePath.generic_string(utf8).c_str(), std::ios::out);
        moduleHeaderFile << m_moduleMainInfosHeader.str();
        moduleHeaderFile.close();
        fs::path moduleSourceFilePath("xpcfGrpcModuleMain.cpp",utf8);
        moduleSourceFilePath = m_folder/moduleSourceFilePath;
        std::ofstream moduleSourceFile(moduleSourceFilePath.generic_string(utf8).c_str(), std::ios::out);
        moduleSourceFile << m_moduleMainDeclareModule.str();
        moduleSourceFile << m_moduleMainInfosSource.str();
        moduleSourceFile.close();
        fs::path findRemakenRulesPath("findremakenrules.pri",utf8);
        findRemakenRulesPath = m_folder/findRemakenRulesPath;
        std::ofstream findRemakenRulesFile(findRemakenRulesPath.generic_string(utf8).c_str(), std::ios::out);
        findRemakenRulesFile << findRemakenRulesStr;
        findRemakenRulesFile.close();
        fs::path pkgDepsPath("packagedependencies.txt",utf8);
        pkgDepsPath = m_folder/pkgDepsPath;
        std::ofstream pkgDepsFile(pkgDepsPath.generic_string(utf8).c_str(), std::ios::out);
        pkgDepsFile << pkgdepsStr;
        if (!metadata[IRPCGenerator::MetadataType::PROJECT_DEPENDENCY_URL].empty()) {
            pkgDepsFile << metadata[IRPCGenerator::MetadataType::PROJECT_DEPENDENCY_URL] << "\n";
        }
        pkgDepsFile.close();
    }
}


