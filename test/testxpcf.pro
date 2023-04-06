TARGET = testxpcf
VERSION=2.6.3

CONFIG += c++1z
CONFIG += console
CONFIG -= qt

DEFINES += MYVERSION=$${VERSION}

#include(findremakenrules.pri)

CONFIG(debug,debug|release) {
    DEFINES += _DEBUG=1
    DEFINES += DEBUG=1
}

CONFIG(release,debug|release) {
    DEFINES += NDEBUG=1
}

macx {
    # EXPERIMENTAL : needs to use remaken configure first
    # REMAKENCONFIG += use_remaken_parser
}
#DEFINES += TEST_XPCF_STD
!contains(DEFINES,TEST_XPCF_STD) {
   DEFINES += TEST_XPCF_BOOST
}

DEFINES += BOOST_ALL_NO_LIB
DEFINES += BOOST_ALL_DYN_LINK

win32:CONFIG -= static
win32:CONFIG += shared
QMAKE_TARGET.arch = x86_64 #must be defined prior to include
DEPENDENCIESCONFIG = sharedlib recurse
#NOTE : CONFIG as staticlib or sharedlib,  DEPENDENCIESCONFIG as staticlib or sharedlib, QMAKE_TARGET.arch and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibconfig.pri inclusion
# include ($${QMAKE_REMAKEN_RULES_ROOT}/templateappconfig.pri)
include (../builddefs/qmake/templateappconfig.pri)
#HEADERS += \
#    TestDefines.h

SOURCES += \
    \   # xpcf_dynamic_test.cpp #\
    #xpcf_properties_test.cpp \
    #xpcf_dynamic_test.cpp #\
    #xpcf_threading_test.cpp 
    #xpcf_xml_test.cpp
    #xpcf_dynamic_test_copy.cpp
#    xpcf_in_code_test.cpp
    xpcf_xml_test.cpp

unix {
    QMAKE_CXXFLAGS += --coverage
    QMAKE_LFLAGS += --coverage
}

linux {
    LIBS += -ldl
}

macx {
    contains(DEFINES,TEST_XPCF_STD) {
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
    }
    QMAKE_MAC_SDK= macosx
    QMAKE_CXXFLAGS += -fasm-blocks -x objective-c++ -std=c++17
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

INCLUDEPATH += $${PWD}/../samples/sample_component

DISTFILES += \
    packagedependencies.txt \
    xpcf_registry_test.xml \
    xpcf_conf_test.xml \
    xpcf_xml_test_valid.xml \
    xpcf_xml_test_missing_start_tag.xml \
    xpcf_xml_test_missing_end_tag.xml \
    xpcf_xml_test_uuid_format_ko.xml

xpcf_xml_files.path = $${USERHOMEFOLDER}/.xpcf
xpcf_xml_files.files = $${PWD}/xpcf_registry_test.xml
xpcf_xml_files.files += $${PWD}/xpcf_conf_test.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_valid.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_missing_start_tag.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_missing_end_tag.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_module_uuid_format_ko.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_alias_uuid_format_ko.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_factory_bindings_to_uuid_format_ko.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_factory_bindings_component_uuid_format_ko.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_factory_inject_uuid_format_ko
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_configuration_uuid_format_ko.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_properties_uuid_format_ko.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_unknown_component.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_property_type_invalid.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_unknown_property.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_property_unknown_component.xml
xpcf_xml_files.files += $${PWD}/xpcf_xml_test_factory_inject_uuid_format_ko.xml

xpcf_xml_sample_structure_files.path = $${PROJECTDEPLOYDIR}/
xpcf_xml_sample_structure_files.files = $${PWD}/xpcf_registry_test.xml

INSTALLS += xpcf_xml_files xpcf_xml_sample_structure_files
