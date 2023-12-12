TARGET = testxpcf_static_deps

CONFIG += shared
DEPENDENCIESCONFIG = staticlib recurse
DEFINES += XPCF_STATIC_DEPS

include(testxpcf.pri)
