TARGET = xpcfSampleComponent-static-deps

REMAKEN_PKGSUBDIR=shared
DEPENDENCIESCONFIG -= sharedlib
DEFINES += XPCF_STATIC_DEPS

include(xpcfSampleComponent.pri)
