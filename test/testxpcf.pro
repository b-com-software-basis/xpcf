TARGET = testxpcf
VERSION=1.0.0

CONFIG += c++17
CONFIG += console
CONFIG -= qt

DEFINES += MYVERSION=$${VERSION}

CONFIG(debug,debug|release) {
    DEFINES += _DEBUG=1
    DEFINES += DEBUG=1
}

CONFIG(release,debug|release) {
    DEFINES += NDEBUG=1
}

DEFINES += BOOST_ALL_NO_LIB
DEFINES += BOOST_ALL_DYN_LINK

win32:CONFIG -= static
win32:CONFIG += shared
QMAKE_TARGET.arch = x86_64 #must be defined prior to include
DEPENDENCIESCONFIG = sharedlib
#NOTE : CONFIG as staticlib or sharedlib,  DEPENDENCIESCONFIG as staticlib or sharedlib, QMAKE_TARGET.arch and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibconfig.pri inclusion
include (../builddefs/qmake/templateappconfig.pri)

HEADERS += \
    TestDefines.h

SOURCES += \
    \   # xpcf_dynamic_test.cpp #\
    xpcf_properties_test.cpp \
    xpcf_dynamic_test.cpp
    #xpcf_dynamic_test_copy.cpp
 #   xpcf_in_code_test.cpp

unix {
    QMAKE_CXXFLAGS += --coverage
    QMAKE_LFLAGS += --coverage
}

macx {
    QMAKE_MAC_SDK= macosx
    QMAKE_CXXFLAGS += -fasm-blocks -x objective-c++ -std=c++17
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

xpcf_xml_files.path = $$(BCOMDEVROOT)/.xpcf/test
xpcf_xml_files.files=$$files($${PWD}/xpcf_registry_test.xml)
INSTALLS += xpcf_xml_files
