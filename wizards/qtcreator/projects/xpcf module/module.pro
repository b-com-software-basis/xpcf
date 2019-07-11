QT       -= core gui
CONFIG -= app_bundle qt

TARGET = %{ModuleName}
FRAMEWORK = $${TARGET}
VERSION=1.0.0
DEFINES +=  $${TARGET}VERSION=\\\"$${VERSION}\\\"

CONFIG += c++1z
CONFIG += shared

unix {
        USERHOMEFOLDER = $$(HOME)
}

win32 {
    USERHOMEFOLDER = $$clean_path($$(USERPROFILE))
    isEmpty(REMAKENDEPSROOTFOLDER) {
        USERHOMEFOLDER = $$clean_path($$(HOMEDRIVE)$$(HOMEPATH))
    }
}

DEPENDENCIESCONFIG = sharedlib recurse
#NOTE : CONFIG as staticlib or sharedlib, DEPENDENCIESCONFIG as staticlib or sharedlib and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibbundle.pri inclusion
@if '%{remakenLocalRules}' === 'true'
include (builddefs/qmake/templatelibconfig.pri)
@else
include ($${USERHOMEFOLDER}/.remaken/rules/qmake/templatelibconfig.pri)
@endif

DEFINES += BOOST_ALL_NO_LIB
DEFINES += BOOST_ALL_DYN_LINK

SOURCES += \
    %{CppFileName}

HEADERS += \
    %{ModuleName}API.h
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

INSTALLS += header_files
DISTFILES += \
    Makefile
