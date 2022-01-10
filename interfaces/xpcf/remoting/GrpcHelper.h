#ifndef GRPCHELPER_H
#define GRPCHELPER_H
#include <memory>
#include <grpcpp/security/credentials.h>
#include <grpcpp/security/server_credentials.h>
#include <grpc/compression.h>
#include <grpcpp/client_context.h>
#include <grpcpp/server_context.h>
#include <boost/algorithm/string.hpp>

namespace org { namespace bcom { namespace xpcf {

typedef enum {
    InsecureChannelCredentials = 0,
    SslCredentials = 1,
    GoogleDefaultCredentials = 2
} grpcCredentials;

typedef enum  {
    client =0x01,
    server = 0x02,
    bidir = 0x03
} grpcCompressDirection;

typedef enum {
    unspecified = 0x00,
    none = 0x10,
    deflate = 0x20,
    gzip=0x40
} grpcCompressType;

typedef struct grpcCompressionInfos {
    grpcCompressType request = unspecified;
    grpcCompressType response = unspecified;
} grpcCompressionInfos;

typedef struct grpcServerCompressionInfos {
    bool force = false;
    grpcCompressType type = unspecified;
} grpcServerCompressionInfos;

static const std::map<std::string, grpcCompressType> translateCompressionConfigMap = {{"none",grpcCompressType::none},
                                                                                     {"unspecified",grpcCompressType::unspecified},
                                                                                     {"deflate",grpcCompressType::deflate},
                                                                                     {"gzip",grpcCompressType::gzip}};

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

    static grpc_compression_algorithm getCompressionType(unsigned long compressMask)
    {
        grpc_compression_algorithm algo = GRPC_COMPRESS_NONE;
        switch(compressMask & 0xF0) {
        case grpcCompressType::deflate : algo = GRPC_COMPRESS_DEFLATE;
            break;
        case grpcCompressType::gzip : algo = GRPC_COMPRESS_GZIP;
            break;
        default: algo = GRPC_COMPRESS_NONE;
            break;
        }
        return algo;
    }


private:
    GrpcHelper() = default;
};

inline grpcCompressionInfos deduceClientCompressionInfo(const grpcCompressionInfos & serviceInfos, const std::string & methodName,
                   const std::map<std::string,grpcCompressionInfos> & methodCompressionInfos)
{
    grpcCompressionInfos infos;
    if (methodCompressionInfos.find(methodName) != methodCompressionInfos.end()) {
        if (methodCompressionInfos.at(methodName).request != grpcCompressType::unspecified) {
            infos.request = methodCompressionInfos.at(methodName).request;
        }
        else {
            infos.request = serviceInfos.request;
        }
        if (methodCompressionInfos.at(methodName).response != grpcCompressType::unspecified) {
            infos.response = methodCompressionInfos.at(methodName).response;
        }
        else {
            infos.response =  serviceInfos.response;
        }
        return infos;
    }
    infos.request = serviceInfos.request;
    infos.response =  serviceInfos.response;
    return infos;
}

inline grpcCompressType translateCompression(const std::string & compressionStr) {
    if (translateCompressionConfigMap.find(compressionStr) == translateCompressionConfigMap.end()) {
        return grpcCompressType::unspecified;
    }
    return translateCompressionConfigMap.at(compressionStr);
}

inline void translateServerConfiguration(const std::string & grpcCompressionLine, grpcServerCompressionInfos & serviceInfos,
                                         std::map<std::string,grpcServerCompressionInfos> & methodCompressionInfos) {
    std::string compressionLine = grpcCompressionLine;
    boost::erase_all(compressionLine, "\r");
    std::vector<std::string> compressDetails;
    boost::split(compressDetails, compressionLine, [&](char c) {return c == '|';});
    if (compressDetails.size() > 1) {
        if (compressDetails[0] == "service") {
             serviceInfos.type = translateCompression(compressDetails[1]);
             if (compressDetails.size() == 3) {
                serviceInfos.force = std::atol(compressDetails[2].c_str());
             }
        }
        if (compressDetails[0] == "method") {
            if (compressDetails.size() > 2) {
               std::string methodName = compressDetails[1];
               xpcf::grpcServerCompressionInfos infos;
               infos.type = translateCompression(compressDetails[2]);
               if (compressDetails.size() == 4) {
                  infos.force = std::atol(compressDetails[3].c_str());
               }
              methodCompressionInfos.insert({methodName, infos});
            }
        }
    }
}

inline void translateClientConfiguration(const std::string & grpcCompressionLine, grpcCompressionInfos & serviceInfos,
                                         std::map<std::string,grpcCompressionInfos> & methodCompressionInfos) {
    std::string compressionLine = grpcCompressionLine;
    boost::erase_all(compressionLine, "\r");
    std::vector<std::string> compressDetails;
    boost::split(compressDetails, compressionLine, [&](char c) {return c == '|';});
    if (compressDetails.size() > 1) {
        if (compressDetails[0] == "service") {
            serviceInfos.request = translateCompression(compressDetails[1]);
            if (compressDetails.size() == 3) {
                serviceInfos.response = translateCompression(compressDetails[2]);
            }
        }
        if (compressDetails[0] == "method") {
            if (compressDetails.size() > 2) {
                std::string methodName = compressDetails[1];
                xpcf::grpcCompressionInfos infos;
                infos.request = translateCompression(compressDetails[2]);
                if (compressDetails.size() == 4) {
                    infos.response = translateCompression(compressDetails[3]);
                }
                methodCompressionInfos.insert({methodName, infos});
            }
        }
    }
}

inline grpcServerCompressionInfos deduceServerCompressionType(grpcCompressType askedCompressType, const grpcServerCompressionInfos & serviceInfos, const std::string & methodName,
                   const std::map<std::string,grpcServerCompressionInfos> & methodCompressionInfos)
{
    grpcServerCompressionInfos infos;
    if (askedCompressType != grpcCompressType::unspecified) {
        infos.type = askedCompressType;
    }
    if (serviceInfos.type != grpcCompressType::unspecified && (serviceInfos.force||askedCompressType == grpcCompressType::unspecified)) {
        infos = serviceInfos;
    }
    if (methodCompressionInfos.find(methodName) != methodCompressionInfos.end()) {
        auto & compressInfo = methodCompressionInfos.at(methodName);
        if (compressInfo.type != grpcCompressType::unspecified && (compressInfo.force||askedCompressType == grpcCompressType::unspecified)) {
            infos = compressInfo;
        }
    }
    return infos;
}


inline grpcCompressType prepareClientCompressionContext(::grpc::ClientContext & context, const grpcCompressionInfos & infos)
{
    // try to set service compression first
    if (infos.request != grpcCompressType::unspecified) {
        context.set_compression_algorithm(xpcf::GrpcHelper::getCompressionType(infos.request));
    }

    if (infos.response != grpcCompressType::unspecified) {
        return infos.response;
    }
    return grpcCompressType::unspecified;
}

inline void prepareServerCompressionContext(::grpc::ServerContext* context, const grpcServerCompressionInfos & infos)
{
    if (infos.type != grpcCompressType::unspecified) {
        context->set_compression_algorithm(xpcf::GrpcHelper::getCompressionType(infos.type));
    }
}


}}}



#endif // GRPCHELPER_H
