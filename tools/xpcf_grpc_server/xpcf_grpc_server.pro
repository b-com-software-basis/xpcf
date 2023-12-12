TARGET = xpcf_grpc_server

LIST = "packagedependencies.txt" "grpc_server_sample.xml" "xpcfGrpcSampleComponentServer.xml"
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

include(../../version.pri)
VERSION = XPCF_VERSION

DEFINES += XPCF_SHARED_DEPS

include(xpcf_grpc_server.pri)