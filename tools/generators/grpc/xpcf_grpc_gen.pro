TARGET = xpcf_grpc_gen
VERSION = 2.6.3

CONFIG += c++1z
CONFIG += console
CONFIG -= qt

DEFINES += MYVERSION=$${VERSION}
DEFINES += MYVERSIONSTRING=\\\"$${VERSION}\\\"

#DEFINES += XPCF_NAMEDINJECTIONAPPROACH

CONFIG(debug,debug|release) {
    DEFINES += _DEBUG=1
    DEFINES += DEBUG=1
    CPPAST_ROOT_BUILD=$${_PRO_FILE_PWD_}/../../../libs/build-cppast-Debug
}

CONFIG(release,debug|release) {
    DEFINES += NDEBUG=1
    CPPAST_ROOT_BUILD=$${_PRO_FILE_PWD_}/../../../libs/build-cppast-Release
}


linux {
    CPPAST_ROOT_BUILD=$${CPPAST_ROOT_BUILD}/linux-gcc
}

macx {
    CPPAST_ROOT_BUILD=$${CPPAST_ROOT_BUILD}/mac-clang
}

exists($${CPPAST_ROOT_BUILD}) {
#note : after llvm upgrade from brew, execution of the xpcf_grpc_gen can show errors such as
# [libclang] [error] /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/11.0.0/include/xmmintrin.h:1934: use of undeclared identifier '__builtin_ia32_storehps'
# in this case, rebuild cppast and xpcf_grpc_gen
    CPPAST_ROOT=$${_PRO_FILE_PWD_}/../../../libs/cppast
    INCLUDEPATH += $${CPPAST_ROOT}/include
    INCLUDEPATH += $${CPPAST_ROOT}/external/cxxopts/include
    INCLUDEPATH += $${CPPAST_ROOT}/external/type_safe/include
    INCLUDEPATH += $${CPPAST_ROOT}/external/type_safe/external/debug_assert
    LIBS += -L$${CPPAST_ROOT_BUILD}/src -lcppast
    LIBS += -L$${CPPAST_ROOT_BUILD} -l_cppast_tiny_process
} else {
    error("cppast root build folder ($${CPPAST_ROOT_BUILD} doesn't exist: create cppast root build folder and build cppast prior to running qmake.$$escape_expand(\\n)To build cppast do :$$escape_expand(\\n)cd " $${_PRO_FILE_PWD_} "/../../../libs/$$escape_expand(\\n)./build_cppast.sh" )
}
win32:CONFIG -= static
win32:CONFIG += shared
QMAKE_TARGET.arch = x86_64 #must be defined prior to include
DEPENDENCIESCONFIG = static recurse
#NOTE : CONFIG as staticlib or sharedlib,  DEPENDENCIESCONFIG as staticlib or sharedlib, QMAKE_TARGET.arch and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibconfig.pri inclusion
include (../../../builddefs/qmake/templateappconfig.pri)

HEADERS += \ \
    ASTParser.h \
    AbstractGenerator.h \
    ClassDescriptor.h \
    CppHelpers.h \
    GRPCFlatBufferGenerator.h \
    GRPCMessage.h \
    MethodDescriptor.h \
    MockGenerator.h \
    ParameterDescriptor.h \
    GRPCProtoGenerator.h \
    ProjectGenerator.h \
    ProxyGenerator.h \
    RemakenPathHelper.h \
    RemoteServiceGenerator.h \
    ServerGenerator.h \
    TypeDescriptor.h \
    XmlHelpers.h \
    XpcfConfigGenerator.h \
    interfaces/IRPCGenerator.h \
    interfaces/ITypeParser.h

SOURCES += \
    ASTParser.cpp \
    AbstractGenerator.cpp \
    ClassDescriptor.cpp \
    GRPCFlatBufferGenerator.cpp \
    GRPCMessage.cpp \
    MethodDescriptor.cpp \
    ParameterDescriptor.cpp \
    GRPCProtoGenerator.cpp \
    MockGenerator.cpp \
    ProjectGenerator.cpp \
    ProxyGenerator.cpp \
    RemakenPathHelper.cpp \
    RemoteServiceGenerator.cpp \
    ServerGenerator.cpp \
    TypeDescriptor.cpp \
    XpcfConfigGenerator.cpp \
    xpcf_grpc_gen.cpp

linux {
    LIBS += -ldl
    LIBS += -L/home/linuxbrew/.linuxbrew/lib # temporary fix caused by grpc with -lre2 ... without -L in grpc.pc
    QMAKE_CXXFLAGS += -fPIC
#    LLVM_BINARIES = /home/linuxbrew/.linuxbrew/opt/llvm/bin
    LLVM_BINARIES = /usr/bin
    LLVM_LIBDIR = $$system($${LLVM_BINARIES}/llvm-config-10 --libdir)
    LLVM_INCDIR = $$system($${LLVM_BINARIES}/llvm-config-10 --includedir)
    LIBS += -L$${LLVM_LIBDIR} -lclang
    LLVM_CLANG_LIBS = $$files($${LLVM_LIBDIR}/libclang*.a)
    LIBS += $${LLVM_CLANG_LIBS}
    QMAKE_LFLAGS += -L$${LLVM_LIBDIR}
    QMAKE_CXXFLAGS += -std=c++17 -I$${LLVM_INCDIR}
    DISTFILES += ../../../libs/build_cppast.sh
}

macx {
    QMAKE_MAC_SDK= macosx

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
    # must be migrated to remaken
# cppast and xpcf_grpc_gen depends upon brew llvm installation
# XCode version update needs sometimes to upgrade brew llvm then to rebuild cppast (with the correct link toward llvm)
# cppast cmake line is :
# cmake -DCPPAST_TEMPLATE_FULLARGUMENTSPARSING=ON -DCMAKE_BUILD_TYPE=Debug -DLLVM_CONFIG_BINARY=/usr/local/opt/llvm/bin/llvm-config ../../cppast/
# Xcode version update needs to set a link from current OS sdk towards MacOSX.sdk cf https://stackoverflow.com/questions/52509602/cant-compile-c-program-on-a-mac-after-upgrade-to-mojave
    LLVM_BINARIES = /usr/local/opt/llvm/bin
    LLVM_LIBDIR = $$system($${LLVM_BINARIES}/llvm-config --libdir)
    LLVM_INCDIR = $$system($${LLVM_BINARIES}/llvm-config --includedir)
    LIBS += -L$${LLVM_LIBDIR} -lclang
    LLVM_CLANG_LIBS = $$files($${LLVM_LIBDIR}/libclang*.a)
    LIBS += $${LLVM_CLANG_LIBS}
    QMAKE_LFLAGS += -L$${LLVM_LIBDIR} -L/usr/local/lib
    QMAKE_CXXFLAGS += -fasm-blocks -x objective-c++ -std=c++17 -I$${LLVM_INCDIR}
    #QMAKE_CXXFLAGS += --coverage
    #QMAKE_LFLAGS += --coverage
}

win32 {
    QMAKE_LFLAGS += /MACHINE:X64
    # DEFINES += WIN64 UNICODE _UNICODE
    QMAKE_COMPILER_DEFINES += _WIN64

    LLVM_LIBDIR = $$system(llvm-config --libdir)
    LLVM_INCDIR = $$system(llvm-config --includedir)
    LIBS += -L\"$${LLVM_LIBDIR}\" -llibclang -lShell32
    # QMAKE_LFLAGS += -L\"$${LLVM_LIBDIR}\"
    QMAKE_CXXFLAGS += -I\"$${LLVM_INCDIR}\"
    DEFINES += _WIN32_WINNT=0x0A00

    # Windows Kit (msvc2013 64)
    # LIBS += -L$$(WINDOWSSDKDIR)lib/winv6.3/um/x64 -lshell32 -lgdi32 -lComdlg32
    # INCLUDEPATH += $$(WINDOWSSDKDIR)lib/winv6.3/um/x64
    # DISTFILES += ../../../libs/build_cppast.bat
}

DISTFILES += packagedependencies.txt


