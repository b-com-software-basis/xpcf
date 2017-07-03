QT       -= core gui
CONFIG -= app_bundle qt

TARGET = xpcf
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

DEFINES += TIXML_USE_STL


SOURCES += \
    src/tinyxml2.cpp \
    src/tinyxmlhelper.cpp \
    src/ComponentMetadata.cpp \
    src/ContainerMetadata.cpp \
    src/InterfaceMetadata.cpp \
    src/MethodMetadata.cpp \
    src/ComponentBase.cpp \
    src/ComponentManager.cpp

HEADERS += \
    src/tinyxml2.h \
    src/tinyxmlhelper.h \
    interfaces/XPCF_NullObject.h \
    interfaces/InterfaceMetadata.h \
    interfaces/ComponentMetadata.h \
    interfaces/ContainerMetadata.h \
    interfaces/MethodMetadata.h \
    interfaces/IComponentIntrospect.h \
    interfaces/ComponentBase.h \
    #interfaces/IComponentManager.h \
    interfaces/XPCF_definitions.h \
    interfaces/ComponentFactory.h \
    interfaces/ContainerFactory.h \
    interfaces/IComponentManager.h \
    src/ComponentManager.h


unix {
}

macx {
    DEFINES += _MACOS_TARGET_
    QMAKE_MAC_SDK= macosx
    QMAKE_CFLAGS += -mmacosx-version-min=10.7 -std=c11 #-x objective-c++
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=c11 -std=c++11 -fPIC#-x objective-c++
    QMAKE_LFLAGS += -mmacosx-version-min=10.7 -v -lstdc++
    LIBS += -lstdc++ -lc -lpthread
}

win32 {
    DEFINES += _X86_VC12_TARGET_
    DEFINES += MBCS _MBCS
	DEFINES += BOOST_ALL_NO_LIB
 }

INCLUDEPATH += $${PWD} $${PWD}/interfaces

header_files.path = $${PROJECTDEPLOYDIR}/interfaces/
header_files.files = $$files($${PWD}/interfaces/*.h)

INSTALLS += header_files
DISTFILES += \
    Makefile
