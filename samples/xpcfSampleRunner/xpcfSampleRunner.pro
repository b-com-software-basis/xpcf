TARGET = xpcfSampleRunner

!exists(packagedependencies.txt) {
    message("$$TARGET - packagedependencies.txt not present, generate it")
    win32 {
        system(../../scripts/win/update_version.bat)
    }
    linux {
        system(../../scripts/unixes/update_version.sh)
    }
}

QT       -= core gui
CONFIG -= app_bundle qt
CONFIG += console

FRAMEWORK = $${TARGET}
VERSION=1.0.0
DEFINES +=  $${TARGET}VERSION=\"$${VERSION}\"

CONFIG += c++1z
CONFIG += shared

DEPENDENCIESCONFIG = sharedlib recurse
#NOTE : CONFIG as staticlib or sharedlib, DEPENDENCIESCONFIG as staticlib or sharedlib and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibbundle.pri inclusion
include (../../builddefs/qmake/templateappconfig.pri)

DEFINES += BOOST_ALL_NO_LIB
DEFINES += BOOST_ALL_DYN_LINK

SOURCES += xpcfSampleRunner_main.cpp

unix {
}

linux {
    LIBS += -ldl
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

INCLUDEPATH += $${PWD} $${_PRO_FILE_PWD_}/../interfaces
DISTFILES +=     Makefile
