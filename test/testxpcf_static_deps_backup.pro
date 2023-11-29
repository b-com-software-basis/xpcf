TARGET = testxpcf_static_deps

CONFIG += shared
DEPENDENCIESCONFIG = sharedlib recurse
DEFINES += BOOST_ALL_DYN_LINK
DEFINES += XPCF_SHARED_DEPS


include(testxpcf.pri)
