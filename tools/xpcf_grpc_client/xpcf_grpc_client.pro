TARGET = xpcf_grpc_client

LIST = "packagedependencies.txt" "grpc_client_sample.xml" "xpcfGrpcSampleComponentClient.xml"
for(file, LIST) {
    !exists(file) { 
        message("$$TARGET - at least $${file} not present, generate it")
        win32 {
            system(../../../scripts/win/update_version.bat)
        }
        linux {
            system(../../../scripts/unixes/update_version.sh)
        }
    }
}

DEFINES += XPCF_SHARED_DEPS

include(xpcf_grpc_client.pri)