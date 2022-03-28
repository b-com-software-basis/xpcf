cc_library(
    name = "xpcf",
    defines = [
        "WITHREMOTING",
        "XPCF_USE_BOOST",
        "BOOST_ALL_NO_LIB",
        "BOOST_ALL_DYN_LINK",
        "XPCFSUBDIRSEARCH=\\\"debug\\\"",
        "TIXML_USE_STL"
    ],
    copts = ["-std=c++17"],
    hdrs = glob(["interfaces/xpcf/**/*.h", "interfaces/xpcf/**/*.hpp", "interfaces/private/xpcf/*.hpp", "interfaces/private/xpcf/*.h"]),
    srcs = ["src/GrpcManager.cpp", "src/AliasManager.cpp","src/Factory.cpp","src/Property.cpp","src/BaseTask.cpp","src/PropertyManager.cpp","src/ComponentBase.cpp","src/InjectableMetadata.cpp","src/PropertyMap.cpp","src/ComponentManager.cpp","src/InterfaceMetadata.cpp","src/Registry.cpp","src/ComponentMetadata.cpp","src/tinyxml2.cpp","src/ConfigurableBase.cpp","src/ModuleManager.cpp","src/tinyxmlhelper.cpp","src/Enumerator.cpp","src/ModuleMetadata.cpp","src/Exception.cpp","src/PathBuilder.cpp"],
    strip_include_prefix = "interfaces",
    linkstatic = False,
    deps = ["@boost//:boost","@grpcpp//:grpcpp"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "testXpcf",
    copts = ["-std=c++17", "-Isamples/sample_component"],
    srcs = ["test/xpcf_dynamic_test.cpp", "test/xpcf_properties_test.cpp",  "test/xpcf_threading_test.cpp", "test/TestDefines.h"],
    deps = ["@boost//:boost","@xpcfSampleComponent//:xpcfSampleComponent"],
)
