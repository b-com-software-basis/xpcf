QT       -= core gui
CONFIG -= app_bundle qt

TARGET = xpcfComponentTemplate
INSTALLSUBDIR = bcomBuild
FRAMEWORK = $${TARGET}
VERSION=1.0.0
DEFINES += MYVERSION=$${VERSION}

CONFIG += c++11
CONFIG += shared

PROJECTDEPLOYDIR = $$(BCOMDEVROOT)/$${INSTALLSUBDIR}/$${FRAMEWORK}/$${VERSION}
DEPENDENCIESCONFIG = sharedlib
#NOTE : CONFIG as staticlib or sharedlib, DEPENDENCIESCONFIG as staticlib or sharedlib and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibbundle.pri inclusion
include (builddefs/qmake/templatelibconfig.pri)

staticlib {
    DEFINES += XPCF_STATIC
} else {
    DEFINES += XPCF_SHARED
}

SOURCES += \
    container_main.cpp \
    ComponentTemplate.cpp


HEADERS += \
    ComponentTemplate.h \
    ITemplateInterface.h


unix {
}

macx {
    DEFINES += _MACOS_TARGET_
    QMAKE_MAC_SDK= macosx
    QMAKE_CFLAGS += -mmacosx-version-min=10.7 -std=c11 #-x objective-c++
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=c11 -std=c++11 -O3 -fPIC#-x objective-c++
    QMAKE_LFLAGS += -mmacosx-version-min=10.7 -v -lstdc++
    LIBS += -lstdc++ -lc -lpthread
}

win32 {
    DEFINES += _X86_VC12_TARGET_
    DEFINES += MBCS _MBCS
 }

INCLUDEPATH += $${PWD}

header_files.path = $${PROJECTDEPLOYDIR}/interfaces/
header_files.files = $$files($${PWD}/I*.h)

INSTALLS += header_files
DISTFILES += \
    Makefile
