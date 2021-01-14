#ifndef HELPERS_H
#define HELPERS_H

#include <map>

#ifndef SWIG
namespace org { namespace bcom { namespace xpcf {

template < typename Key, typename T> bool mapContains(const std::map<Key,T> & mapContainer, Key k)
{
    if (mapContainer.find(k) != mapContainer.end()) {
        return true;
    }
    return false;
}


class ScopeExit {
public:
    ScopeExit(const std::function<void(void)> & f):m_func(f) {}
    ~ScopeExit () {
        m_func();
    }
private:
    const std::function<void(void)> & m_func;
};

}}}
#endif // SWIG

#ifndef SWIG
    #define ATTRIBUTE(X) [[X]]
    #define ATTRIB_1ARG(X, ARG) [[X(ARG)]]
    #define XPCF_IGNORE [[xpcf::ignore]]
    #define XPCF_IN [[xpcf::in]]
    #define XPCF_OUT [[xpcf::out]]
    #define XPCF_INOUT [[xpcf::inout]]
    #define XPCF_CLIENTUUID(uuid_string) [[xpcf::clientUUID(uuid_string)]]
    #define XPCF_SERVERUUID(uuid_string) [[xpcf::serverUUID(uuid_string)]]
    #define XPCF_GRPC_RPCNAME(rpcNameString) [[grpc::rpcName(rpcNameString)]]
    #define XPCF_GRPC_REQUEST(requestNameString) [[grpc::request(requestNameString)]]
    #define XPCF_GRPC_RESPONSE(responseNameString) [[grpc::response(responseNameString)]]
    #define XPCF_GRPC_STREAMING [[grpc::streaming]]
    #define XPCF_GRPC_CLIENT_STREAMING [[grpc::client_streaming]]
    #define XPCF_GRPC_SERVER_STREAMING [[grpc::server_streaming]]
#else
    #define ATTRIBUTE(X)
    #define ATTRIB_1ARG(X, ARG)
    #define XPCF_IGNORE
    #define XPCF_IN
    #define XPCF_OUT
    #define XPCF_INOUT
    #define XPCF_CLIENTUUID(uuid_string)
    #define XPCF_SERVERUUID(uuid_string)
    #define XPCF_GRPC_RPCNAME(rpcNameString)
    #define XPCF_GRPC_REQUEST(requestNameString)
    #define XPCF_GRPC_RESPONSE(responseNameString)
    #define XPCF_GRPC_STREAMING
    #define XPCF_GRPC_CLIENT_STREAMING
    #define XPCF_GRPC_SERVER_STREAMING
#endif

#endif // HELPERS_H
