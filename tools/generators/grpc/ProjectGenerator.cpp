#include "ProjectGenerator.h"

namespace xpcf = org::bcom::xpcf;

template<> ProjectGenerator * xpcf::ComponentFactory::createInstance<ProjectGenerator>();



ProjectGenerator::ProjectGenerator():AbstractGenerator(xpcf::toMap<ProjectGenerator>())
{
}


ProjectGenerator::~ProjectGenerator()
{

}

std::map<IRPCGenerator::MetadataType,std::string> ProjectGenerator::generate(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata)
{

    return metadata;
}


