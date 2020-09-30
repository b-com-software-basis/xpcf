#include "RemoteServiceGenerator.h"

namespace xpcf = org::bcom::xpcf;

template<> RemoteServiceGenerator * xpcf::ComponentFactory::createInstance<RemoteServiceGenerator>();



RemoteServiceGenerator::RemoteServiceGenerator():AbstractGenerator(xpcf::toMap<RemoteServiceGenerator>())
{
     declareInjectable<IRPCGenerator>(m_grpcGenerator, "grpc");
     declareInjectable<IRPCGenerator>(m_proxyGenerator, "proxy");
     declareInjectable<IRPCGenerator>(m_serverGenerator, "server");
     declareInjectable<IRPCGenerator>(m_projectGenerator, "project");
}


RemoteServiceGenerator::~RemoteServiceGenerator()
{

}

void RemoteServiceGenerator::setDestinationFolder(const std::string & folder)
{
    AbstractGenerator::setDestinationFolder(folder);
    m_grpcGenerator->setDestinationFolder(folder);
    m_proxyGenerator->setDestinationFolder(folder);
    m_serverGenerator->setDestinationFolder(folder);
    m_projectGenerator->setDestinationFolder(folder);
}

void RemoteServiceGenerator::setGenerateMode(const GenerateMode & mode)
{
    AbstractGenerator::setGenerateMode(mode);
    m_grpcGenerator->setGenerateMode(mode);
    m_proxyGenerator->setGenerateMode(mode);
    m_serverGenerator->setGenerateMode(mode);
    m_projectGenerator->setGenerateMode(mode);
}

std::map<IRPCGenerator::MetadataType,std::string> RemoteServiceGenerator::generate(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata)

{
    // generation chain is ordered : metadata from grpc generator must be forwarded to proxy, server and project generators
    metadata = m_grpcGenerator->generate(c, metadata);
    metadata = m_proxyGenerator->generate(c, metadata);
    metadata = m_serverGenerator->generate(c, metadata);
    metadata =  m_projectGenerator->generate(c, metadata);
    return metadata;
}

