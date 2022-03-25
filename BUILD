cc_library(
    name = "xpcf",
    defines = [
#        "WITHREMOTING",
        "XPCF_USE_BOOST",
        "BOOST_ALL_NO_LIB",
        "XPCFSUBDIRSEARCH=\\\"debug\\\"",
        "TIXML_USE_STL"
    ],
    hdrs = glob(["interfaces/xpcf/**/*.h", "interfaces/xpcf/**/*.hpp", "interfaces/private/xpcf/*.hpp", "interfaces/private/xpcf/*.h"]),
    srcs = glob(["src/*.cpp"]),
    strip_include_prefix = "interfaces",
    linkstatic = 0,
    deps = ["@boost//:boost"],
    visibility = ["//visibility:public"]
)
