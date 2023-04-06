QT       -= core gui
CONFIG -= app_bundle qt

TARGET = xpcf-std
FRAMEWORK = $${TARGET}
VERSION=2.6.3
DEFINES += XPCFVERSION=\\\"$${VERSION}\\\"

CONFIG += c++1z
CONFIG += shared

macx {
    #CONFIG += use_brew_llvm
    # howto setup conan to use brew llvm ?
}

DEPENDENCIESCONFIG = staticlib
#NOTE : CONFIG as staticlib or sharedlib, DEPENDENCIESCONFIG as staticlib or sharedlib and PROJECTDEPLOYDIR MUST BE DEFINED BEFORE templatelibbundle.pri inclusion
include (builddefs/qmake/templatelibconfig.pri)

staticlib {
    DEFINES += XPCF_STATIC
} else {
    DEFINES += XPCF_SHARED
}

CONFIG(debug,debug|release) {
#    DEFINES += XPCF_WITH_LOGS
    DEFINES += "XPCFDEBUG"
    DEFINES += XPCFSUBDIRSEARCH=\\\"debug\\\"
}

CONFIG(release,debug|release) {
    DEFINES += XPCFSUBDIRSEARCH=\\\"release\\\"
}

DEFINES += BOOST_ALL_NO_LIB

DEFINES += TIXML_USE_STL

SOURCES += \
    src/AliasManager.cpp \
    src/Factory.cpp \
    src/InjectableMetadata.cpp \
    src/Registry.cpp \
    src/tinyxml2.cpp \
    src/tinyxmlhelper.cpp \
    src/ComponentMetadata.cpp \
    src/InterfaceMetadata.cpp \
    src/MethodMetadata.cpp \
    src/ComponentBase.cpp \
    src/ComponentManager.cpp \
    src/ModuleMetadata.cpp \
    src/PathBuilder.cpp \
    src/ModuleManager.cpp \
    src/Enumerator.cpp \
    src/Property.cpp \
    src/PropertyMap.cpp \
    src/Exception.cpp \
    src/PropertyManager.cpp \
    src/ConfigurableBase.cpp \
    src/BaseTask.cpp

HEADERS += \
    interfaces/swig/xpcf_api.i \
    interfaces/swig/xpcf_collection.i \
    interfaces/swig/xpcf_component.i \
    interfaces/swig/xpcf_core.i \
    interfaces/swig/xpcf_module.i \
    interfaces/swig/xpcf_properties.i \
    interfaces/swig/xpcf_threading.i \
    interfaces/swig/xpcf_traits.i \
    interfaces/xpcf/api/InjectableMetadata.h \
    interfaces/xpcf/core/helpers.h \
    interfaces/xpcf/threading/MultiConsumer.h \
    src/AliasManager.h \
    src/Factory.h \
    src/Registry.h \
    src/tinyxml2.h \
    src/tinyxmlhelper.h \
    src/ComponentManager.h \
    src/PathBuilder.h \
    src/ModuleManager.h \
    src/Enumerator.h \
    src/Collection.h \
    src/Property.h \
    src/PropertyMap.h \
    src/PropertySequenceWrapper.h \
    src/PropertyWrapper.h \
    src/PropertyManager.h \
    src/BlockEnumerator.h \
    interfaces/xpcf/core/Result.h \
    interfaces/xpcf/core/Exception.h \
    interfaces/xpcf/core/XPCFErrorCode.h \
    interfaces/xpcf/core/refs.h \
    interfaces/xpcf/core/traits.h \
    interfaces/xpcf/core/uuid.h \
    interfaces/xpcf/api/InterfaceTraits.h \
    interfaces/xpcf/api/IComponentIntrospect.h \
    interfaces/xpcf/api/IComponentManager.h \
    interfaces/xpcf/api/IModuleManager.h \
    interfaces/xpcf/api/IConfigurable.h \
    interfaces/xpcf/api/ComponentMetadata.h \
    interfaces/xpcf/api/ModuleMetadata.h \
    interfaces/xpcf/api/InterfaceMetadata.h \
    interfaces/xpcf/api/IInjectable.h \
    interfaces/xpcf/component/ComponentBase.h \
    interfaces/xpcf/component/ComponentFactory.h \
    interfaces/xpcf/component/ComponentTraits.h \
    interfaces/xpcf/component/ConfigurableBase.h \
    interfaces/xpcf/module/IModuleIndex.h \
    interfaces/xpcf/module/ModuleFactory.h \
    interfaces/xpcf/properties/IProperty.h \
    interfaces/xpcf/properties/IPropertyMap.h \
    interfaces/xpcf/collection/IEnumerable.h \
    interfaces/xpcf/collection/IEnumerator.h \
    interfaces/xpcf/collection/ICollection.h \
    interfaces/xpcf/xpcf.h \
    interfaces/xpcf/xpcf_api_define.h \
    interfaces/xpcf/threading/BaseTask.h \
    interfaces/xpcf/threading/SharedBuffer.h \
    interfaces/xpcf/threading/SharedFifo.h \
    interfaces/xpcf/threading/ITask.h \
    interfaces/xpcf/threading/SharedCircularBuffer.h \
    interfaces/xpcf/threading/FiberFifos.h \
    interfaces/xpcf/threading/SharedLifo.h \
    interfaces/xpcf/threading/DropBuffer.h

contains(DEFINES, "WITHREMOTING") {
SOURCES += \
    src/GrpcManager.cpp

HEADERS += \
    src/GrpcManager.h \
    interfaces/xpcf/remoting/IGrpcServerManager.h \
    interfaces/xpcf/remoting/IGrpcService.h
}

linux {
    QMAKE_LFLAGS += -ldl
}

macx {
    DEFINES += _MACOS_TARGET_
    QMAKE_MAC_SDK= macosx
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15
    QMAKE_CFLAGS += -mmacosx-version-min=10.14 #-x objective-c++
    QMAKE_CXXFLAGS += -mmacosx-version-min=10.14  -std=c++17 -fPIC#-x objective-c++
    QMAKE_LFLAGS += -mmacosx-version-min=10.14 -v -lstdc++
    LIBS += -lstdc++ -lc -lpthread
}

win32 {
    DEFINES += _X86_VC12_TARGET_
    DEFINES += MBCS _MBCS
 }

INCLUDEPATH += $${PWD} $${PWD}/interfaces $${PWD}/libs/stduuid/include
include(builddefs/qmake/code_analysis/remaken_code_scanner.prf)

h_api_files.path = $${PROJECTDEPLOYDIR}/interfaces/xpcf/api
h_api_files.files = $$files($${PWD}/interfaces/xpcf/api/*)
h_collection_files.path = $${PROJECTDEPLOYDIR}/interfaces/xpcf/collection
h_collection_files.files = $$files($${PWD}/interfaces/xpcf/collection/*)
h_component_files.path = $${PROJECTDEPLOYDIR}/interfaces/xpcf/component
h_component_files.files = $$files($${PWD}/interfaces/xpcf/component/*)
h_core_files.path = $${PROJECTDEPLOYDIR}/interfaces/xpcf/core
h_core_files.files = $$files($${PWD}/interfaces/xpcf/core/*)
h_module_files.path = $${PROJECTDEPLOYDIR}/interfaces/xpcf/module
h_module_files.files = $$files($${PWD}/interfaces/xpcf/module/*)
h_properties_files.path = $${PROJECTDEPLOYDIR}/interfaces/xpcf/properties
h_properties_files.files = $$files($${PWD}/interfaces/xpcf/properties/*)
h_threading_files.path = $${PROJECTDEPLOYDIR}/interfaces/xpcf/threading
h_threading_files.files = $$files($${PWD}/interfaces/xpcf/threading/*)
h_xpcf_files.path = $${PROJECTDEPLOYDIR}/interfaces/xpcf
h_xpcf_files.files = $${PWD}/interfaces/xpcf/xpcf.h
h_xpcf_files.files += $${PWD}/interfaces/xpcf/xpcf_api_define.h



INSTALLS += h_api_files h_collection_files h_component_files h_core_files h_module_files h_properties_files h_threading_files h_xpcf_files
!contains(DEFINES, "XPCF_USE_BOOST") {
    h_uuid_files.path = $${PROJECTDEPLOYDIR}/interfaces/xpcf/core/stduuid
    h_uuid_files.files = $$files($${PWD}/libs/stduuid/include/*)
    INSTALLS += h_uuid_files
}

contains(DEFINES, "WITHREMOTING") {
    h_remoting_files.path = $${PROJECTDEPLOYDIR}/interfaces/xpcf/remoting
    h_remoting_files.files = $$files($${PWD}/interfaces/xpcf/remoting/*)
    INSTALLS += h_remoting_files
}

message("LFLAGS="$${QMAKE_LFLAGS})
DISTFILES += \
    Makefile \
    doc/xpcf-registry-sample.xml
