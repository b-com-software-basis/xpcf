QT       -= core gui
CONFIG -= app_bundle qt

TARGET = %{ModuleName}
VERSION=1.0.0
FRAMEWORK = $${TARGET}

DEFINES += MYVERSION=$${VERSION}

CONFIG += c++1z
CONFIG += shared
CONFIG += console
CONFIG -= qt

DEPENDENCIESCONFIG = sharedlib recurse
#NOTE : CONFIG as staticlib or sharedlib, DEPENDENCIESCONFIG as staticlib or sharedlib and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibbundle.pri inclusion
include (builddefs/qmake/templatelibconfig.pri)

DEFINES += BOOST_ALL_NO_LIB
DEFINES += BOOST_ALL_DYN_LINK

SOURCES += \
    main.cpp

HEADERS += \
 
unix {
}

macx {
    DEFINES += _MACOS_TARGET_
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
    QMAKE_MAC_SDK= macosx
    QMAKE_CFLAGS += -mmacosx-version-min=10.7 #-x objective-c++
    QMAKE_CXXFLAGS += -fasm-blocks -mmacosx-version-min=10.7 -std=c++17 -fPIC#-x objective-c++
    QMAKE_LFLAGS += -mmacosx-version-min=10.7 -v -lstdc++
    LIBS += -lstdc++ -lc -lpthread
}

win32 {
        QMAKE_LFLAGS += /MACHINE:X64
    DEFINES += WIN64 UNICODE _UNICODE
    QMAKE_COMPILER_DEFINES += _WIN64

    # Windows Kit (msvc2013 64)
    LIBS += -L$$(WINDOWSSDKDIR)lib/winv6.3/um/x64 -lshell32 -lgdi32 -lComdlg32
    INCLUDEPATH += $$(WINDOWSSDKDIR)lib/winv6.3/um/x64
 }

INCLUDEPATH += $${PWD}

DISTFILES += \
    Makefile
        
