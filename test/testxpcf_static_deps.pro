TARGET = testxpcf_static_deps

# CONFIG -= shared
# DEPENDENCIESCONFIG = staticlib recurse
# DEFINES += XPCF_STATIC_DEPS
CONFIG += shared
DEPENDENCIESCONFIG = staticlib recurse
#DEFINES += BOOST_ALL_DYN_LINK
DEFINES += XPCF_STATIC_DEPS


include(testxpcf.pri)
