#include "RemoteServiceGenerator.h"

namespace xpcf = org::bcom::xpcf;

template<> RemoteServiceGenerator * xpcf::ComponentFactory::createInstance<RemoteServiceGenerator>();



RemoteServiceGenerator::RemoteServiceGenerator():AbstractGenerator(xpcf::toMap<RemoteServiceGenerator>())
{
    declareInjectable<IRPCGenerator>(m_grpcGenerator, "grpc", true);
    declareInjectable<IRPCGenerator>(m_proxyGenerator, "proxy", true);
    declareInjectable<IRPCGenerator>(m_serverGenerator, "server", true);
    declareInjectable<IRPCGenerator>(m_projectGenerator, "project", true);
}


RemoteServiceGenerator::~RemoteServiceGenerator()
{

}

void RemoteServiceGenerator::setDestinationFolder(const std::string & folder)
{
    AbstractGenerator::setDestinationFolder(folder);
    if (m_grpcGenerator) {
        m_grpcGenerator->setDestinationFolder(folder);
    }
    if (m_proxyGenerator) {
        m_proxyGenerator->setDestinationFolder(folder);
    }
    if (m_serverGenerator) {
        m_serverGenerator->setDestinationFolder(folder);
    }
    if (m_projectGenerator) {
        m_projectGenerator->setDestinationFolder(folder);
    }
}

void RemoteServiceGenerator::setGenerateMode(const GenerateMode & mode)
{
    AbstractGenerator::setGenerateMode(mode);
    if (m_grpcGenerator) {
        m_grpcGenerator->setGenerateMode(mode);
    }
    if (m_proxyGenerator) {
        m_proxyGenerator->setGenerateMode(mode);
    }
    if (m_serverGenerator) {
        m_serverGenerator->setGenerateMode(mode);
    }
    if (m_projectGenerator) {
        m_projectGenerator->setGenerateMode(mode);
    }
}

std::map<IRPCGenerator::MetadataType,std::string> RemoteServiceGenerator::generateImpl(ClassDescriptor & c, std::map<MetadataType,std::string> metadata)

{
    std::string generationStep;
    // generation chain is ordered : metadata from grpc generator must be forwarded to proxy, server and project generators
    try {
    if (m_grpcGenerator) {
        generationStep = "GRPC proto files";
        metadata = m_grpcGenerator->generate(c, metadata);
    }
    if (m_proxyGenerator) {
        generationStep = "GRPC C++ client proxy files";
        metadata = m_proxyGenerator->generate(c, metadata);
    }
    if (m_serverGenerator) {
        generationStep = "GRPC C++ server files";
        metadata = m_serverGenerator->generate(c, metadata);
    }
    if (m_projectGenerator) {
        generationStep = "GRPC C++ project files";
        metadata = m_projectGenerator->generate(c, metadata);
    }
    }
    catch (const GenerationException & e) {
        std::cerr << "Error generating " << generationStep << " " << e.what();
    }
    return metadata;
}

std::map<IRPCGenerator::MetadataType,std::string> RemoteServiceGenerator::validateImpl(const ClassDescriptor & c, std::map<MetadataType,std::string> metadata)
{
    if (m_grpcGenerator) {
        metadata = m_grpcGenerator->validate(c, metadata);
    }
    if (m_proxyGenerator) {
        metadata = m_proxyGenerator->validate(c, metadata);
    }
    if (m_serverGenerator) {
        metadata = m_serverGenerator->validate(c, metadata);
    }
    if (m_projectGenerator) {
        metadata = m_projectGenerator->validate(c, metadata);
    }
    return metadata;
}

void RemoteServiceGenerator::finalizeImpl(std::map<MetadataType,std::string> metadata)
{
    if (m_grpcGenerator) {
        m_grpcGenerator->finalize(metadata);
    }
    if (m_proxyGenerator) {
        m_proxyGenerator->finalize(metadata);
    }
    if (m_serverGenerator) {
        m_serverGenerator->finalize(metadata);
    }
    if (m_projectGenerator) {
        m_projectGenerator->finalize(metadata);
    }
}
