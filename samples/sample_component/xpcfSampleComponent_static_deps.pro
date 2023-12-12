TARGET = xpcfSampleComponent_static_deps

!exists(packagedependencies.txt) {
    message("$$TARGET - packagedependencies.txt not present, generate it")
    win32 {
        system(../../scripts/win/update_version.bat)
    }
    linux {
        system(../../scripts/unixes/update_version.sh)
    }
}

REMAKEN_PKGSUBDIR=shared
DEPENDENCIESCONFIG -= sharedlib
DEFINES += XPCF_STATIC_DEPS

include(xpcfSampleComponent.pri)
