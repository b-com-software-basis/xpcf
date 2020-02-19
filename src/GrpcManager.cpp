#include "GrpcManager.h"

namespace xpcf = org::bcom::xpcf;


namespace org { namespace bcom { namespace xpcf {
template<> GrpcManager * xpcf::ComponentFactory::createInstance<GrpcManager>();



GrpcManager::GrpcManager():xpcf::ConfigurableBase(xpcf::toMap<GrpcManager>())
{
    declareInterface<IGrpcServerManager>(this);
    //  Inject declarations come here : declare any component that must be injected to your component through its interface /////////////////////////..///// declareInjectable<IFilter>(m_filter);
    //
    // Inject declaration can have a name :
    // declareInjectable<IFilter>(m_blurFilter, "blurFilter");
    //
    // Inject declaration can be optional i.e. not finding a binding component for the interface is not an error :
    // declareInjectable<IImageFilter>(m_imageFilter, false);

    // wrap any component member variable to expose as properties with declareProperty<T>() with T matching the variable type
    // For example : declareProperty<float>("blurFactor",m_blurFactor);
    declareProperty("ServerAddress",m_serverAddress);//also works with template type deduction when m_memberVariable is a supported type of IProperty
}


GrpcManager::~GrpcManager()
{

}

void GrpcManager::unloadComponent ()
{
    // provide component cleanup strategy
    // default strategy is to delete self, uncomment following line in this case :
    // delete this;
    return;
}

xpcf::XPCFErrorCode GrpcManager::onConfigured()
{
    // Add custom onConfigured code
    return xpcf::XPCFErrorCode::_SUCCESS;
}

grpc::ServerBuilder & GrpcManager::registerService(grpc::Service * service)
{
    m_serverBuilder.RegisterService(service);
    return m_serverBuilder;
}

grpc::ServerBuilder & GrpcManager::registerService(const grpc::string & host, grpc::Service * service)
{
    m_serverBuilder.RegisterService(host,service);
    return m_serverBuilder;
}

grpc::ServerBuilder & GrpcManager::registerService(SRef<IGrpcService> service) {
    m_serverBuilder.RegisterService(service->getService());
    return m_serverBuilder;
}

grpc::ServerBuilder & GrpcManager::registerService(const grpc::string & host, SRef<IGrpcService> service){
    m_serverBuilder.RegisterService(host,service->getService());
    return m_serverBuilder;
}

void GrpcManager::runServer()
{
    std::unique_ptr<grpc::Server> server(m_serverBuilder.BuildAndStart());
    std::cout << "Server listening on " << m_serverAddress << std::endl;
    server->Wait();
}

}}}
