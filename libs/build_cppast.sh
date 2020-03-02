#!/bin/bash

BUILDFOLDER=build-cppast

build_cppast() {
    BUILDTYPE=$1
    if [ ! -d ${BUILDFOLDER}-${BUILDTYPE} ]; then
        echo "Creating ${BUILDFOLDER}-${BUILDTYPE} folder"
        mkdir -p ${BUILDFOLDER}-${BUILDTYPE}
        echo "Building  cppast in ${BUILDTYPE} mode"
        cd ${BUILDFOLDER}-${BUILDTYPE} && cmake -DCMAKE_BUILD_TYPE=${BUILDTYPE} -DLLVM_CONFIG_BINARY=/usr/local/opt/llvm/bin/llvm-config ../cppast/ && make && cd -
    fi
}

build_cppast "Debug"
build_cppast "Release"
