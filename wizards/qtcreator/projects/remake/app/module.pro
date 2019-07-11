QT       -= core gui
CONFIG -= app_bundle qt

TARGET = %{ModuleName}
INSTALLSUBDIR = bcomBuild
FRAMEWORK = $${TARGET}
VERSION=1.0.0
DEFINES += MYVERSION=$${VERSION}

CONFIG += c++17
CONFIG += shared

DEPENDENCIESCONFIG = sharedlib recurse
#NOTE : CONFIG as staticlib or sharedlib, DEPENDENCIESCONFIG as staticlib or sharedlib and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibbundle.pri inclusion
include (builddefs/qmake/templatelibconfig.pri)

DEFINES += BOOST_ALL_NO_LIB
DEFINES += BOOST_ALL_DYN_LINK

SOURCES += \
    %{CppFileName}

HEADERS += \
    %{ModuleName}API.h \
    %{ModuleName}_traits.h
 
unix {
}

macx {
    DEFINES += _MACOS_TARGET_
    QMAKE_MAC_SDK= macosx
    QMAKE_CFLAGS += -mmacosx-version-min=10.7 #-x objective-c++
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.7 -std=c++17 -fPIC#-x objective-c++
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
header_files.files += _traits.h

INSTALLS += header_files
DISTFILES += \
    Makefile

SOURCES += \\
        
