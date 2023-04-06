## remove Qt dependencies
QT       -= core gui
CONFIG -= qt

## global defintions : target lib name, version
TARGET = swig_xpcf
FRAMEWORK = $${TARGET}
VERSION=2.6.3

DEFINES += MYVERSION=$${VERSION}
DEFINES += TEMPLATE_LIBRARY
CONFIG += c++1z


DEFINES += USE_XPCF_BOOST
CONFIG(debug,debug|release) {
    DEFINES += _DEBUG=1
    DEFINES += DEBUG=1
}

CONFIG(release,debug|release) {
    DEFINES += _NDEBUG=1
    DEFINES += NDEBUG=1
}

SWIG_CXX_GEN_FOLDER=swig-xpcf-cxx
SWIG_CSHARP_GEN_FOLDER=csharp

!exists($${SWIG_CXX_GEN_FOLDER}) {
    error("Call build_swig_csharp.sh script before building this project")
}

DEPENDENCIESCONFIG = sharedlib recursive install

PROJECTCONFIG = QTVS

#NOTE : CONFIG as staticlib or sharedlib, DEPENDENCIESCONFIG as staticlib or sharedlib, QMAKE_TARGET.arch and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibconfig.pri inclusion
include (../../builddefs/qmake/templatelibconfig.pri)

## DEFINES FOR MSVC/INTEL C++ compilers
msvc {
DEFINES += "_BCOM_SHARED=__declspec(dllexport)"
}

include (swig_xpcf.pri)

DEFINES += SWIG

unix:!android {
    QMAKE_CXXFLAGS += -Wignored-qualifiers
}

linux {
   QMAKE_LFLAGS += -ldl
}

macx {
    DEFINES += _MACOS_TARGET_
    QMAKE_MAC_SDK= macosx
    QMAKE_CFLAGS += -mmacosx-version-min=10.7 -std=c11 #-x objective-c++
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=c11 -std=c++11 -O3 -fPIC#-x objective-c++
    QMAKE_LFLAGS += -mmacosx-version-min=10.7 -v -lstdc++
    LIBS += -lstdc++ -lc -lpthread -L/usr/local/lib
}

win32 {

    DEFINES += WIN64 UNICODE _UNICODE
    QMAKE_COMPILER_DEFINES += _WIN64
    QMAKE_CXXFLAGS += -wd4250 -wd4251 -wd4244 -wd4275 /Od
}

android {
    ANDROID_ABIS="arm64-v8a"
}

OTHER_FILES += \
    packagedependencies.txt \
    ../../interfaces/swig/XPCF_Api.i \
    ../../interfaces/swig/XPCF_Collection.i \
    ../../interfaces/swig/XPCF_Component.i \
    ../../interfaces/swig/XPCF_Core.i \
    ../../interfaces/swig/XPCF_Module.i \
    ../../interfaces/swig/XPCF_Properties.i \
    ../../interfaces/swig/XPCF_Threading.i \
    ../../interfaces/swig/XPCF_Traits.i \
    ../../interfaces/swig/swig.i

#NOTE : Must be placed at the end of the .pro
include (../../builddefs/qmake/remaken_install_target.pri)))

DISTFILES += \
    build_swig_csharp.sh
