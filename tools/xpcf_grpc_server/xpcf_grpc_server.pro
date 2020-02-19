TARGET = xpcf_grpc_server
VERSION = 1.0.0

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
DEPENDENCIESCONFIG = static recurse
#NOTE : CONFIG as staticlib or sharedlib,  DEPENDENCIESCONFIG as staticlib or sharedlib, QMAKE_TARGET.arch and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibconfig.pri inclusion
include (../../builddefs/qmake/templateappconfig.pri)

HEADERS += \
    GrpcServerManager.h


SOURCES += \
    GrpcServerManager.cpp\
    xpcf_grpc_server.cpp


linux {
    LIBS += -ldl
}

macx {
    QMAKE_MAC_SDK= macosx
    QMAKE_CXXFLAGS += -fasm-blocks -x objective-c++ -std=c++17
    # must be migrated to remaken
    CPPAST_ROOT=$$(HOME)/workspace/labs/DIT/SFT/github/github-cppast
    CPPAST_ROOT_BUILD=$$(HOME)/workspace/labs/DIT/SFT/github/build-cppast
    INCLUDEPATH += $${CPPAST_ROOT}/include
    INCLUDEPATH += $${CPPAST_ROOT}/external/cxxopts/include
    INCLUDEPATH += $${CPPAST_ROOT}/external/type_safe/include
    INCLUDEPATH += $${CPPAST_ROOT}/external/type_safe/external/debug_assert
    LIBS += -L$${CPPAST_ROOT_BUILD}/debug/src -lcppast
    LIBS += -L$${CPPAST_ROOT_BUILD}/debug -l_cppast_tiny_process
# cppast and xpcf_grpc_gen depends upon brew llvm installation
# XCode version update needs sometimes to upgrade brew llvm then to rebuild cppast (with the correct link toward llvm)
# cppast cmake line is :
# cmake -DCPPAST_TEMPLATE_FULLARGUMENTSPARSING=ON -DCMAKE_BUILD_TYPE=Debug -DLLVM_CONFIG_BINARY=/usr/local/opt/llvm/bin/llvm-config ../../github-cppast/
# Xcode version update needs to set a link from current OS sdk towards MacOSX.sdk cf https://stackoverflow.com/questions/52509602/cant-compile-c-program-on-a-mac-after-upgrade-to-mojave
    LLVM_BINARIES = /usr/local/opt/llvm/bin
    LLVM_LIBDIR = $$system($${LLVM_BINARIES}/llvm-config --libdir)
    LIBS += -L$${LLVM_LIBDIR} -lclang
    LLVM_CLANG_LIBS = $$files($${LLVM_LIBDIR}/libclang*.a)
    LIBS += $${LLVM_CLANG_LIBS}
    #QMAKE_CXXFLAGS += --coverage
    #QMAKE_LFLAGS += --coverage
}

win32 {
    QMAKE_LFLAGS += /MACHINE:X64
    DEFINES += WIN64 UNICODE _UNICODE
    QMAKE_COMPILER_DEFINES += _WIN64

    # Windows Kit (msvc2013 64)
    LIBS += -L$$(WINDOWSSDKDIR)lib/winv6.3/um/x64 -lshell32 -lgdi32 -lComdlg32
    INCLUDEPATH += $$(WINDOWSSDKDIR)lib/winv6.3/um/x64
}

DISTFILES += \
    packagedependencies.txt


