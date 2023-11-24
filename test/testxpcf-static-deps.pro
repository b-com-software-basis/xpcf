TARGET = testxpcf-static-deps

CONFIG -= shared
DEPENDENCIESCONFIG = staticlib recurse
DEFINES += XPCF_STATIC_DEPS

include(testxpcf.pri)
