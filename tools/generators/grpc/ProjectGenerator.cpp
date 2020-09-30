#include "ProjectGenerator.h"

namespace xpcf = org::bcom::xpcf;

template<> ProjectGenerator * xpcf::ComponentFactory::createInstance<ProjectGenerator>();



ProjectGenerator::ProjectGenerator():AbstractGenerator(xpcf::toMap<ProjectGenerator>())
{
    m_headerProjectInfos << "HEADERS +=\\";
    m_srcProjectInfos << "SOURCES +=\\";
    m_protoProjectInfos << "PROTO =\\";
}


ProjectGenerator::~ProjectGenerator()
{
    if (m_mode == GenerateMode::STD_COUT) {
        std::cout << m_protoProjectInfos.str();
        std::cout << m_headerProjectInfos.str();
        std::cout << m_srcProjectInfos.str();
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
    }
}

std::map<IRPCGenerator::MetadataType,std::string> ProjectGenerator::generate(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata)
{
    m_headerProjectInfos << metadata.at(MetadataType::HEADERFILENAME) << "\\\n";
    m_srcProjectInfos << metadata.at(MetadataType::CPPFILENAME) << "\\\n";
    m_protoProjectInfos << metadata.at(MetadataType::GRPCPROTOFILENAME) << "\\\n";
    return metadata;
}


