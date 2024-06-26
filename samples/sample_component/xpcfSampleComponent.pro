TARGET = xpcfSampleComponent

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
DEPENDENCIESCONFIG = sharedlib
DEFINES += XPCF_SHARED_DEPS

include(xpcfSampleComponent.pri)
