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

include(../../version.pri)
VERSION = XPCF_VERSION

CONFIG += c++1z
CONFIG += console
CONFIG -= qt

DEFINES += MYVERSION=$${VERSION}
DEFINES += WITHREMOTING

CONFIG(debug,debug|release) {
    DEFINES += _DEBUG=1
    DEFINES += DEBUG=1
    CPPAST_ROOT_BUILD=$${_PRO_FILE_PWD_}/../../libs/build-cppast-Debug
}

CONFIG(release,debug|release) {
    DEFINES += NDEBUG=1
    CPPAST_ROOT_BUILD=$${_PRO_FILE_PWD_}/../../libs/build-cppast-Release
}

linux {
    CPPAST_ROOT_BUILD=$${CPPAST_ROOT_BUILD}/linux-gcc
}

macx {
    CPPAST_ROOT_BUILD=$${CPPAST_ROOT_BUILD}/mac-clang
}

win32:CONFIG -= static
win32:CONFIG += shared
QMAKE_TARGET.arch = x86_64 #must be defined prior to include
DEPENDENCIESCONFIG = shared install_recurse
#NOTE : CONFIG as staticlib or sharedlib,  DEPENDENCIESCONFIG as staticlib or sharedlib, QMAKE_TARGET.arch and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibconfig.pri inclusion
include (../../builddefs/qmake/templateappconfig.pri)

SOURCES += \
    xpcf_grpc_client.cpp


linux {
    LIBS += -ldl
    LIBS += -L/home/linuxbrew/.linuxbrew/lib # temporary fix caused by grpc with -lre2 ... without -L in grpc.pc
}


macx {
    DEFINES += _MACOS_TARGET_
    QMAKE_MAC_SDK= macosx
    QMAKE_CFLAGS += -mmacosx-version-min=10.7 #-x objective-c++
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7  -std=c++17 -fPIC#-x objective-c++
    QMAKE_LFLAGS += -mmacosx-version-min=10.7 -v -lstdc++
    LIBS += -lstdc++ -lc -lpthread
    LIBS += -L/usr/local/lib
}

win32 {
    QMAKE_LFLAGS += /MACHINE:X64
    DEFINES += WIN64 UNICODE _UNICODE
    QMAKE_COMPILER_DEFINES += _WIN64

    # Windows Kit (msvc2013 64)
    LIBS += -L$$(WINDOWSSDKDIR)lib/winv6.3/um/x64 -lshell32 -lgdi32 -lComdlg32
    INCLUDEPATH += $$(WINDOWSSDKDIR)lib/winv6.3/um/x64
}

INCLUDEPATH += $${CPPAST_ROOT_BUILD}/_deps/cxxopts-src/include
message(SLETODO $$INCLUDEPATH)
INCLUDEPATH += ../../samples/sample_component

DISTFILES += \
    grpc_server_sample.xml \
    packagedependencies.txt \
    xpcfGrpcSampleComponentClient.xml

xml_files.path = $${PROJECTDEPLOYDIR}
xml_files.files =  xpcfGrpcSampleComponentClient.xml

xpcf_grpc_xml_files.path = $${USERHOMEFOLDER}/.xpcf/grpc
xpcf_grpc_xml_files.files = xpcfGrpcSampleComponentClient.xml

INSTALLS += xml_files xpcf_grpc_xml_files


