#!/bin/bash

QTVERSION=5.15.2
XPCFROOT=..

display_usage() { 
	echo "This script builds testxpcf."
    echo "It can receive two optional arguments." 
	echo -e "\nUsage: \$0 [path to xpcf project root | default='${XPCFROOT}'] [Qt kit version to use | default='${QTVERSION}'] \n" 
} 

# check whether user had supplied -h or --help . If yes display usage 
if [[ ( $1 == "--help") ||  $1 == "-h" ]] 
then 
    display_usage
    exit 0
fi 

if [ $# -ge 1 ]; then
	XPCFROOT=$1
fi
if [ $# -eq 2 ]; then
	QTVERSION=$2
fi
# default linux values

QMAKE_PATH=$HOME/Qt/${QTVERSION}/gcc_64/bin
QMAKE_SPEC=linux-g++

if [[ "$OSTYPE" == "darwin"* ]]; then
# overload for mac values
	QMAKE_PATH=/Applications/Qt/${QTVERSION}/clang_64/bin
	QMAKE_SPEC=macx-clang
fi

if [ ! -d ${QMAKE_PATH} ]; then
	echo "Qt path '${QMAKE_PATH}' doesn't exist : check your Qt installation and kits"
	exit 2
fi

if [ ! -d ${XPCFROOT} ]; then
	echo "XPCF project root path '${XPCFROOT}' doesn't exist"
	exit 2
fi
echo "XPCF project root path used is : ${XPCFROOT}"

if [ -d build-xpcf/testxpcf ]; then
	rm -rf build-xpcf/testxpcf
fi
if [ -d build-xpcf/samplecomponent ]; then
	rm -rf build-xpcf/samplecomponent
fi
mkdir -p build-xpcf/testxpcf/debug
mkdir -p build-xpcf/testxpcf/release
mkdir -p build-xpcf/samplecomponent/debug
mkdir -p build-xpcf/samplecomponent/release

echo "===========> building XPCF sample component <==========="
pushd build-xpcf/samplecomponent/debug
`${QMAKE_PATH}/qmake ../../../${XPCFROOT}/samples/sample_component/xpcfSampleComponent.pro -spec ${QMAKE_SPEC} CONFIG+=debug CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd
pushd build-xpcf/samplecomponent/release
`${QMAKE_PATH}/qmake ../../../${XPCFROOT}/samples/sample_component/xpcfSampleComponent.pro -spec ${QMAKE_SPEC} CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd
echo "===========> building testxpcf <==========="
pushd build-xpcf/testxpcf/debug
`${QMAKE_PATH}/qmake ../../../${XPCFROOT}/test/testxpcf.pro -spec ${QMAKE_SPEC} CONFIG+=debug CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd
pushd build-xpcf/testxpcf/release
`${QMAKE_PATH}/qmake ../../../${XPCFROOT}/test/testxpcf.pro -spec ${QMAKE_SPEC} CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd


