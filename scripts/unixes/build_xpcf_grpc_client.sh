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

PROJECTNAME=xpcf_grpc_client
BUILDROOTFOLDER=build-${PROJECTNAME}

if [ -d ${BUILDROOTFOLDER}/shared ]; then
	rm -rf ${BUILDROOTFOLDER}/shared
fi
mkdir -p ${BUILDROOTFOLDER}/shared/debug
mkdir -p ${BUILDROOTFOLDER}/shared/release
echo "===========> building XPCF command line interface xpcfcli <==========="
pushd ${BUILDROOTFOLDER}/shared/debug
`${QMAKE_PATH}/qmake ../../../${XPCFROOT}/tools/cli/${PROJECTNAME}.pro -spec ${QMAKE_SPEC} CONFIG+=debug CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd
pushd ${BUILDROOTFOLDER}/shared/release
`${QMAKE_PATH}/qmake ../../../${XPCFROOT}/tools/cli/${PROJECTNAME}.pro -spec ${QMAKE_SPEC} CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd

