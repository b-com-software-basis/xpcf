TARGET = xpcfcli

!exists(packagedependencies.txt) {
    message("$$TARGET - packagedependencies.txt not present, generate it")
    win32 {
        system(../../scripts/win/update_version.bat)
    }
    linux {
        system(../../scripts/unixes/update_version.sh)
    }
}

! exists(../../version.pri) {
    win32 {
        system(../../scripts/win/update_version.bat -t ../../version.pri.template)
    }
    linux {
        system(../../scripts/unixes/update_version.sh -t ../../version.pri.template)
    }
    system(cd $${currentPath})
} 
include(../../version.pri)

FRAMEWORK = xpcfcli
VERSION=$${XPCF_VERSION}

CONFIG += c++17
CONFIG += console
CONFIG -= qt

DEFINES += MYVERSION=$${VERSION}
DEFINES += MYVERSIONSTRING=\\\"$${VERSION}\\\"

CONFIG(debug,debug|release) {
    DEFINES += _DEBUG=1
    DEFINES += DEBUG=1
}

CONFIG(release,debug|release) {
    DEFINES += NDEBUG=1
}

DEFINES += BOOST_ALL_NO_LIB
#DEFINES += BOOST_ALL_DYN_LINK

win32:CONFIG -= static
win32:CONFIG += shared
QMAKE_TARGET.arch = x86_64 #must be defined prior to include
DEPENDENCIESCONFIG = staticlib recurse
#NOTE : CONFIG as staticlib or sharedlib,  DEPENDENCIESCONFIG as staticlib or sharedlib, QMAKE_TARGET.arch and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibconfig.pri inclusion
include (../../builddefs/qmake/templateappconfig.pri)

HEADERS += \

SOURCES += \
    xpcfcli.cpp

unix {
}

macx {
    QMAKE_MAC_SDK= macosx
    QMAKE_CXXFLAGS += -fasm-blocks -x objective-c++
    LIBS += -L/usr/local/lib # temporary fix caused by grpc with -lre2 ... without -L in grpc.pc
}

win32 {
    QMAKE_LFLAGS += /MACHINE:X64
    DEFINES += WIN64 UNICODE _UNICODE
    QMAKE_COMPILER_DEFINES += _WIN64

    # Windows Kit (msvc2013 64)
    LIBS += -L$$(WINDOWSSDKDIR)lib/winv6.3/um/x64 -lshell32 -lgdi32 -lComdlg32
    INCLUDEPATH += $$(WINDOWSSDKDIR)lib/winv6.3/um/x64
}

INCLUDEPATH += $${PWD}/../sample_component

DISTFILES += \
    xpcf_registry_test.xml
