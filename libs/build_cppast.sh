#!/bin/bash

BUILDFOLDER=build-cppast

#!/bin/bash
LLVMBREWPATH=/usr/local
TARGETPLATFORM=mac-clang
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    LLVMBREWPATH=/home/linuxbrew/.linuxbrew
    TARGETPLATFORM=linux-gcc
    CPPASTCMAKECOMPILER="-DCMAKE_CXX_COMPILER=clang++"
    if test ! $(which brew); then
	echo "brew is not installed, first install brew then relaunch this script\n";
	exit 1
    fi
    echo "installing llvm"
#    brew install llvm
    # ...
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # Mac OSX
    if test ! $(which brew); then
	echo "brew is not installed, first install brew then relaunch this script\n";
	exit 1
    fi
    echo "installing llvm"
    brew install llvm
    #next line is mandatory for configure to find the include and libraries files from the command line
    #NOTE : once compilation is done, you should unlink with brew unlink openssl to avoid conflict in macosX
    echo "Before compilation, do \nbrew link --force openssl\nOnce compilation is finalized do\nbrew unlink openssl\n to avoid conflict with mac OS X openssl version\n"
elif [[ "$OSTYPE" == "cygwin" ]]; then
    # POSIX compatibility layer and Linux environment emulation for Windows
    echo "cygwin build not available for the moment"
elif [[ "$OSTYPE" == "msys" ]]; then
    # Lightweight shell and GNU utilities compiled for Windows (part of MinGW)
        echo "msys build not available for the moment"
elif [[ "$OSTYPE" == "win32" ]]; then
    # I'm not sure this can happen.
            echo "win32 build not available for the moment"
elif [[ "$OSTYPE" == "freebsd"* ]]; then
    # ...
                echo "freebsd build not available for the moment"
else
    # Unknown.
                    echo "Unknown OS"
fi

build_cppast() {
    BUILDTYPE=$1
    BUILDPATH=${BUILDFOLDER}-${BUILDTYPE}/${TARGETPLATFORM}
    if [ ! -d ${BUILDPATH} ]; then
        echo "Creating ${BUILDFOLDER}-${BUILDTYPE} folder"
        mkdir -p ${BUILDPATH}
        echo "Building  cppast in ${BUILDTYPE} mode"
	 
        pushd ${BUILDPATH} && cmake -DCMAKE_BUILD_TYPE=${BUILDTYPE} ${CPPASTCMAKECOMPILER} -DLLVM_CONFIG_BINARY=${LLVMBREWPATH}/opt/llvm/bin/llvm-config ../../cppast/ && make && popd
    fi
}

build_cppast "Debug"
build_cppast "Release"
