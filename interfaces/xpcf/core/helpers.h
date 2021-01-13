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
    #define DSL_XPCF_IGNORE [[xpcf::ignore]]
    #define DSL_XPCF_IN [[xpcf::in]]
    #define DSL_XPCF_OUT [[xpcf::out]]
    #define DSL_XPCF_INOUT [[xpcf::inout]]
    #define DSL_XPCF_CLIENTUUID(uuid_string) [[xpcf::clientUUID(uuid_string)]]
    #define DSL_XPCF_SERVERUUID(uuid_string) [[xpcf::serverUUID(uuid_string)]]
    #define DSL_GRPC_RPCNAME(rpcNameString) [[grpc::rpcName(rpcNameString)]]
    #define DSL_GRPC_REQUEST(requestNameString) [[grpc::request(requestNameString)]]
    #define DSL_GRPC_RESPONSE(responseNameString) [[grpc::response(responseNameString)]]
    #define DSL_GRPC_STREAMING [[grpc::streaming]]
    #define DSL_GRPC_CLIENT_STREAMING [[grpc::client_streaming]]
    #define DSL_GRPC_SERVER_STREAMING [[grpc::server_streaming]]
#else
    #define ATTRIBUTE(X)
    #define DSL_XPCF_IGNORE
    #define DSL_XPCF_IN
    #define DSL_XPCF_OUT
    #define DSL_XPCF_INOUT
    #define DSL_XPCF_CLIENTUUID(uuid_string)
    #define DSL_XPCF_SERVERUUID(uuid_string)
    #define DSL_GRPC_RPCNAME(rpcNameString)
    #define DSL_GRPC_REQUEST(requestNameString)
    #define DSL_GRPC_RESPONSE(responseNameString)
    #define DSL_GRPC_STREAMING
    #define DSL_GRPC_CLIENT_STREAMING
    #define DSL_GRPC_SERVER_STREAMING
#endif

#endif // HELPERS_H
