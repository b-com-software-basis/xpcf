cc_library(
    name = "xpcfSampleComponent",
    defines = [
        "BOOST_ALL_NO_LIB",
        "BOOST_ALL_DYN_LINK",
        "XPCFSUBDIRSEARCH=\\\"debug\\\"",
        "TIXML_USE_STL"
    ],
    copts = ["-std=c++17"],
    hdrs = glob(["*.h"]),
    srcs = ["Guitar.cpp", "HumanMusician.cpp", "Ibanez7ElectricGuitar.cpp", "JacksonDinky6ElectricGuitar.cpp", "TakamineFolkGuitar.cpp", "Tuner.cpp", "VirtualGuitarist.cpp"],
    linkstatic = False,
    deps = ["@boost//:boost","@xpcf//:xpcf"],
    visibility = ["//visibility:public"],
)