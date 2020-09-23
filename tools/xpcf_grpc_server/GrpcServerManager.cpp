#include "GrpcServerManager.h"

namespace org { namespace bcom { namespace xpcf {

GrpcServerManager::GrpcServerManager():ConfigurableBase(toMap<GrpcServerManager>())
{
    declareInterface<IGrpcServerManager>(this);
    declareProperty("server_address",m_serverAddress);
    declareInjectable<IGrpcService>(m_services);
}


GrpcServerManager::~GrpcServerManager()
{

}

void GrpcServerManager::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    // delete this;
    return;
}

void GrpcServerManager::registerService(grpc::Service * service)
{
    builder.RegisterService(service);
}

void GrpcServerManager::registerService(const grpc::string & host, grpc::Service * service)
{
    builder.RegisterService(host, service);
}

void GrpcServerManager::registerService(SRef<IGrpcService> service)
{
    registerService(service->getService());
}

void GrpcServerManager::registerService(const grpc::string & host, SRef<IGrpcService> service)
{
    registerService(host, service->getService());
}

void GrpcServerManager::runServer()
{
    builder.AddListeningPort(m_serverAddress, grpc::InsecureServerCredentials());
    for (auto service: *m_services) {
        registerService(service);
    }
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << m_serverAddress << std::endl;
    server->Wait();
}

}}}
