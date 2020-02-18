QT       -= core gui
CONFIG -= app_bundle qt

TARGET = %{ApplicationName}
VERSION=1.0.0
DEFINES +=  $${TARGET}VERSION=\\\"$${VERSION}\\\"

CONFIG += c++1z
CONFIG += console

include(findremakenrules.pri)

CONFIG += shared

@if '%{dependenciesBuildMode}' === 'shared'
DEPENDENCIESCONFIG = sharedlib
@else
DEPENDENCIESCONFIG = staticlib
@endif
@if '%{recurseDependencies}' === 'recurse' && '%{dependenciesInstallMode}' !== 'install_recurse'
DEPENDENCIESCONFIG += recurse
@endif
@if '%{dependenciesInstallMode}' !== 'noinstall'
DEPENDENCIESCONFIG += %{dependenciesInstallMode}
@endif

## Configuration for Visual Studio to install binaries and dependencies. Work also for QT Creator by replacing QMAKE_INSTALL
@if '%{withQTVS}' && '%{withQTVS}' === 'QTVS'
PROJECTCONFIG = QTVS
@endif

#NOTE : CONFIG as staticlib or sharedlib, DEPENDENCIESCONFIG as staticlib or sharedlib and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibbundle.pri inclusion
@if '%{remakenRules}' === 'local'
include (builddefs/qmake/templateappconfig.pri)
@else
include ($${REMAKEN_RULES_ROOT}/templateappconfig.pri)
@endif

#DEFINES += BOOST_ALL_NO_LIB
DEFINES += BOOST_ALL_DYN_LINK
DEFINES += BOOST_AUTO_LINK_NOMANGLE
DEFINES += BOOST_LOG_DYN_LINK

HEADERS += \

SOURCES += \
    %{CppFileName}

unix {
    LIBS += -ldl
    QMAKE_CXXFLAGS += -DBOOST_LOG_DYN_LINK
}

macx {
    QMAKE_MAC_SDK= macosx
    QMAKE_CXXFLAGS += -fasm-blocks -x objective-c++
}

win32 {
    QMAKE_LFLAGS += /MACHINE:X64
    DEFINES += WIN64 UNICODE _UNICODE
    QMAKE_COMPILER_DEFINES += _WIN64
    QMAKE_CXXFLAGS += -wd4250 -wd4251 -wd4244 -wd4275

    # Windows Kit (msvc2013 64)
    LIBS += -L$$(WINDOWSSDKDIR)lib/winv6.3/um/x64 -lshell32 -lgdi32 -lComdlg32
    INCLUDEPATH += $$(WINDOWSSDKDIR)lib/winv6.3/um/x64
 }

INCLUDEPATH += $${PWD}

DISTFILES += \
    Makefile

OTHER_FILES += \
    packagedependencies.txt

@if '%{withQTVS}' && '%{withQTVS}' === 'QTVS'
#NOTE : Must be placed at the end of the .pro
    @if '%{remakenRules}' === 'local'
include (builddefs/qmake/remaken_install_target.pri)
    @else
include ($${REMAKEN_RULES_ROOT}/remaken_install_target.pri)
    @endif
@endif
