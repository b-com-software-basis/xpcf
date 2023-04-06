QT       -= core gui
CONFIG -= app_bundle qt

TARGET = xpcfSampleComponent
FRAMEWORK = $${TARGET}
VERSION=2.6.3
DEFINES += MYVERSION=$${VERSION}

CONFIG += c++17
CONFIG += shared
#DEFINES += USE_XPCF_STD

staticlib {
    REMAKEN_PKGSUBDIR=static
} else {
    REMAKEN_PKGSUBDIR=shared
}

CONFIG(debug,debug|release) {
    REMAKEN_PKGSUBDIR=$${REMAKEN_PKGSUBDIR}/debug/$${TARGET}_$${VERSION}
}

CONFIG(release,debug|release) {
    REMAKEN_PKGSUBDIR=$${REMAKEN_PKGSUBDIR}/release/$${TARGET}_$${VERSION}
}

package_remaken {
    message("Preparing remaken package installation in $${REMAKEN_PKGSUBDIR}")
    INSTALLSUBDIR=$${REMAKEN_PKGSUBDIR}
}

macx {
    # EXPERIMENTAL : needs to use remaken configure first
    # REMAKENCONFIG += use_remaken_parser
}
DEPENDENCIESCONFIG = shared
!contains(DEFINES,USE_XPCF_STD) {
   DEFINES += USE_XPCF_BOOST
   DEPENDENCIESCONFIG += install_recurse
}
#NOTE : CONFIG as staticlib or sharedlib, DEPENDENCIESCONFIG as staticlib or sharedlib and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibbundle.pri inclusion
include (../../builddefs/qmake/templatelibconfig.pri)

DEFINES += BOOST_ALL_NO_LIB
DEFINES += BOOST_ALL_DYN_LINK

SOURCES += \
    Ibanez7ElectricGuitar.cpp \
    TakamineFolkGuitar.cpp \
    Tuner.cpp \
    xpcfSampleComponent_main.cpp \
    HumanMusician.cpp \
    VirtualGuitarist.cpp \
    Guitar.cpp \
    JacksonDinky6ElectricGuitar.cpp

HEADERS += \
    Ibanez7ElectricGuitar.h \
    TakamineFolkGuitar.h \
    Tuner.h \
    xpcfSampleComponentAPI.h \
    xpcfSampleComponent_traits.h \
    IHuman.h \
    IMusician.h \
    IGuitarist.h \
    HumanMusician.h \
    VirtualGuitarist.h \
    IGuitar.h \
    Guitar.h \
    JacksonDinky6ElectricGuitar.h
 
unix {
}

linux {
    LIBS += -L/home/linuxbrew/.linuxbrew/lib # temporary fix caused by grpc with -lre2 ... without -L in grpc.pc
}


macx {
    contains(DEFINES,USE_XPCF_STD) {
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
    }
    DEFINES += _MACOS_TARGET_
    QMAKE_MAC_SDK= macosx
    QMAKE_CFLAGS += -mmacosx-version-min=10.7 #-x objective-c++
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=c++17 -fPIC#-x objective-c++
    QMAKE_LFLAGS += -mmacosx-version-min=10.7 -v -lstdc++
    LIBS += -lstdc++ -lc -lpthread
    LIBS += -L/usr/local/lib
}

win32 {
    DEFINES += _X86_VC12_TARGET_
    DEFINES += MBCS _MBCS
 }

INCLUDEPATH += $${PWD}

header_files.path = $${PROJECTDEPLOYDIR}/interfaces/
header_files.files = $$files($${PWD}/I*.h)
header_files.files += xpcfSampleComponent_traits.h

INSTALLS += header_files
DISTFILES += \
    Makefile \
    swig/XPCF_SampleComponent.i
