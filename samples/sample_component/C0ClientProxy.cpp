#include <iostream>
#include "I0.h"
#include "I0.pb.h"
#include "I0.grpc.pb.h"

#include "GrpcManager.h"

using namespace std;

    virtual void I0_opname () = 0
{
    
    GrpcManager::getInstance().initialize();
    I0::I0Service_Stub * grpcStub =  GrpcManager::getInstance().getServiceStub();
    I0::I0_opnameRequest request;
    I0::I0_opnameResponse response;
        try{
        grpcStub->I0_opnameStub(request, &response, GrpcManager::getInstance().getTimeOut());
        cout << response.DebugString() << endl;
                return static_cast<enum WBRETURNCODES>(response.result());
    } catch (grpc::rpc_error &e) {
        cout << "Error: " << e.what() << endl;
    }
} //    virtual void I0_opname () = 0


    virtual void I0_opname2 () = 0
{
    
    GrpcManager::getInstance().initialize();
    I0::I0Service_Stub * grpcStub =  GrpcManager::getInstance().getServiceStub();
    I0::I0_opname2Request request;
    I0::I0_opname2Response response;
        try{
        grpcStub->I0_opname2Stub(request, &response, GrpcManager::getInstance().getTimeOut());
        cout << response.DebugString() << endl;
                return static_cast<enum WBRETURNCODES>(response.result());
    } catch (grpc::rpc_error &e) {
        cout << "Error: " << e.what() << endl;
    }
} //    virtual void I0_opname2 () = 0


    virtual void I0_opname3 () = 0
{
    
    GrpcManager::getInstance().initialize();
    I0::I0Service_Stub * grpcStub =  GrpcManager::getInstance().getServiceStub();
    I0::I0_opname3Request request;
    I0::I0_opname3Response response;
        try{
        grpcStub->I0_opname3Stub(request, &response, GrpcManager::getInstance().getTimeOut());
        cout << response.DebugString() << endl;
                return static_cast<enum WBRETURNCODES>(response.result());
    } catch (grpc::rpc_error &e) {
        cout << "Error: " << e.what() << endl;
    }
} //    virtual void I0_opname3 () = 0


    virtual void I0_opname4 () = 0
{
    
    GrpcManager::getInstance().initialize();
    I0::I0Service_Stub * grpcStub =  GrpcManager::getInstance().getServiceStub();
    I0::I0_opname4Request request;
    I0::I0_opname4Response response;
        try{
        grpcStub->I0_opname4Stub(request, &response, GrpcManager::getInstance().getTimeOut());
        cout << response.DebugString() << endl;
                return static_cast<enum WBRETURNCODES>(response.result());
    } catch (grpc::rpc_error &e) {
        cout << "Error: " << e.what() << endl;
    }
} //    virtual void I0_opname4 () = 0


    virtual void I0_opname5 () = 0
{
    
    GrpcManager::getInstance().initialize();
    I0::I0Service_Stub * grpcStub =  GrpcManager::getInstance().getServiceStub();
    I0::I0_opname5Request request;
    I0::I0_opname5Response response;
        try{
        grpcStub->I0_opname5Stub(request, &response, GrpcManager::getInstance().getTimeOut());
        cout << response.DebugString() << endl;
                return static_cast<enum WBRETURNCODES>(response.result());
    } catch (grpc::rpc_error &e) {
        cout << "Error: " << e.what() << endl;
    }
} //    virtual void I0_opname5 () = 0


