#!/bin/bash

QTVERSION=5.15.2
XPCFROOT=../..

display_usage() { 
	echo "This script builds xpcf in shared mode."
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

if [ -d build-xpcf/shared ]; then
	rm -rf build-xpcf/shared
fi
mkdir -p build-xpcf/shared/debug
mkdir -p build-xpcf/shared/release
echo "===========> building XPCF shared <==========="
pushd build-xpcf/shared/debug
`${QMAKE_PATH}/qmake ../../../${XPCFROOT}/xpcf.pro -spec ${QMAKE_SPEC} CONFIG+=debug CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd
pushd build-xpcf/shared/release
`${QMAKE_PATH}/qmake ../../../${XPCFROOT}/xpcf.pro -spec ${QMAKE_SPEC} CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd

