TARGET = testxpcf_static_deps

REMAKEN_PKGSUBDIR=shared
DEPENDENCIESCONFIG -= sharedlib
DEPENDENCIESCONFIG += recurse
DEFINES += XPCF_STATIC_DEPS

include(testxpcf.pri)
