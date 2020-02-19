#include "GrpcServerManager.h"

namespace org { namespace bcom { namespace xpcf {

GrpcServerManager::GrpcServerManager():ConfigurableBase(toMap<GrpcServerManager>())
{
    declareInterface<IGrpcServerManager>(this);
    declareProperty("server_address",m_serverAddress);
}


GrpcServerManager::~GrpcServerManager()
{

}

XPCFErrorCode GrpcServerManager::onConfigured()
{
    builder.AddListeningPort(m_serverAddress, grpc::InsecureServerCredentials());
    return ConfigurableBase::onConfigured();
}

void GrpcServerManager::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    // delete this;
    return;
}

grpc::ServerBuilder & GrpcServerManager::registerService(grpc::Service * service)
{
    builder.RegisterService(service);
}

grpc::ServerBuilder & GrpcServerManager::registerService(const grpc::string & host, grpc::Service * service)
{
}

grpc::ServerBuilder & GrpcServerManager::registerService(SRef<IGrpcService> service)
{
    registerService(service->getService());
}

grpc::ServerBuilder & GrpcServerManager::registerService(const grpc::string & host, SRef<IGrpcService> service)
{
    registerService(service->getService());
}

void GrpcServerManager::runServer()
{
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << m_serverAddress << std::endl;
    server->Wait();
}

}}}
