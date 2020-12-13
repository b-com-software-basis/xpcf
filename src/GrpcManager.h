#ifndef GRPCMANAGER_H
#define GRPCMANAGER_H
#include <xpcf/component/ConfigurableBase.h>
#include <xpcf/remoting/IGrpcServerManager.h>

namespace org { namespace bcom { namespace xpcf {

class GrpcManager: public org::bcom::xpcf::ConfigurableBase,
        virtual public IGrpcServerManager
{
public:
    GrpcManager();
    ~GrpcManager() override;
    void unloadComponent () override;

    void registerService(grpc::Service * service) override;
    void registerService(const grpc::string & host, grpc::Service * service) override;
    void registerService(SRef<IGrpcService> service) override;
    void registerService(const grpc::string & host, SRef<IGrpcService> service) override;

    void runServer() override;

    org::bcom::xpcf::XPCFErrorCode onConfigured() override;

private:
    std::string m_serverAddress;
    grpc::ServerBuilder m_serverBuilder;
};

}}}

template <> struct org::bcom::xpcf::ComponentTraits<org::bcom::xpcf::GrpcManager>
{
    static constexpr const char * UUID = "{cda197f0-db7a-4922-b280-ebf53f4fe790}";
    static constexpr const char * NAME = "GrpcManager";
    static constexpr const char * DESCRIPTION = "GrpcManager implements None interface";
};
#endif // GRPCMANAGER_H
