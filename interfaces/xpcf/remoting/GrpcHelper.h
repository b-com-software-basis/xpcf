#ifndef GRPCHELPER_H
#define GRPCHELPER_H
#include <memory>
#include <grpcpp/security/credentials.h>
#include <grpcpp/security/server_credentials.h>

namespace org { namespace bcom { namespace xpcf {

typedef enum {
  InsecureChannelCredentials = 0,
  SslCredentials = 1,
  GoogleDefaultCredentials = 2
} grpcCredentials;

class GrpcHelper
{
public:
     static std::shared_ptr< ::grpc::ChannelCredentials > getCredentials(grpcCredentials channelCredentials) {
      switch (channelCredentials) {
      case GoogleDefaultCredentials:
      return ::grpc::GoogleDefaultCredentials();
      case SslCredentials:
      return ::grpc::SslCredentials(::grpc::SslCredentialsOptions());
      case InsecureChannelCredentials:
      return ::grpc::InsecureChannelCredentials();
      default:
      return nullptr;
      }
      return nullptr;
    }

     static std::shared_ptr< ::grpc::ServerCredentials > getServerCredentials(grpcCredentials serverCredentials) {
      switch (serverCredentials) {
      case GoogleDefaultCredentials:
      return nullptr;
      case SslCredentials:
      return ::grpc::SslServerCredentials(::grpc::SslServerCredentialsOptions());
      case InsecureChannelCredentials:
      return ::grpc::InsecureServerCredentials();
      default:
      return nullptr;
      }
      return nullptr;
    }


private:
     GrpcHelper() = default;
};

}}}

#endif // GRPCHELPER_H
