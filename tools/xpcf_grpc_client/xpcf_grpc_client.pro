TARGET = xpcf_grpc_client
VERSION = 2.6.3

CONFIG += c++1z
CONFIG += console
CONFIG -= qt

DEFINES += MYVERSION=$${VERSION}
DEFINES += WITHREMOTING

CONFIG(debug,debug|release) {
    DEFINES += _DEBUG=1
    DEFINES += DEBUG=1
}

CONFIG(release,debug|release) {
    DEFINES += NDEBUG=1
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

INCLUDEPATH += ../../libs/cppast/external/cxxopts/include
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


