#!/bin/bash

QTVERSION=5.15.2
PROJECTROOT=../..
MODE=shared
QMAKE_MODE=""

# default linux values
QMAKE_PATH=$HOME/Qt/${QTVERSION}/gcc_64/bin
QMAKE_SPEC=linux-g++
if [[ "$OSTYPE" == "darwin"* ]]; then
	# overload for mac values
	QMAKE_PATH=/Application/Qt/${QTVERSION}/clang_64/bin
	QMAKE_SPEC=macx-clang
fi

display_usage() { 
	echo "This script builds a remaken project."
    echo "It takes the project name as first argument and can receive four optional arguments." 
	echo -e "\nUsage: \$0 [project name] [ build mode {shared|static} | default='${MODE}' ] [path to project root | default='${PROJECTROOT}'] [Qt kit version to use | default='${QTVERSION}'] [path to qmake | default='${QMAKEPATH}'] \n" 
} 

# check whether user had supplied -h or --help . If yes display usage 
if [[ ( $1 == "--help") ||  $1 == "-h" || $# -eq 0 ]] 
then 
    display_usage
    exit 0
fi 

if [ $# -ge 1 ]; then
	PROJECTNAME=$1
fi
if [ $# -ge 2 ]; then
	MODE=$2
fi
if [ $# -ge 3 ]; then
	PROJECTROOT=$3
fi
if [ $# -ge 4 ]; then
	QTVERSION=$4
fi
if [ $# -eq 5 ]; then
	QMAKE_PATH=$5
fi

if [[ ${MODE} != "shared" && ${MODE} != "static" ]]; then
	echo "mode must be either shared or static - ${MODE} is an unsupported value"
	exit 2
fi

if [[ ${MODE} == "static" ]]; then
	QMAKE_MODE=CONFIG+=staticlib
	echo "${MODE} build enabled: building with ${QMAKE_MODE} option"
fi

if [ ! -d ${QMAKE_PATH} ]; then
	echo "Qt path '${QMAKE_PATH}' doesn't exist : check your Qt installation and kits"
	exit 2
fi

if [ ! -d ${PROJECTROOT} ]; then
	echo "Project root path '${PROJECTROOT}' doesn't exist"
	exit 2
fi
if [ ! -e ${PROJECTROOT}/${PROJECTNAME}.pro ]; then
	echo "Project path '${PROJECTROOT}/${PROJECTNAME}.pro' doesn't exist"
	echo "Trying  ${PROJECTROOT}/${PROJECTNAME}/${PROJECTNAME}.pro"
	if [ -e ${PROJECTROOT}/${PROJECTNAME}/${PROJECTNAME}.pro ]; then
		PROJECTROOT=${PROJECTROOT}/${PROJECTNAME}
	else
		echo "Project path '${PROJECTROOT}/${PROJECTNAME}/${PROJECTNAME}.pro' doesn't exist"
		exit 2
	fi
fi
if [[ ! ${PROJECTROOT:0:1} = / ]]; then
	PROJECTROOT=../../../${PROJECTROOT}
fi
echo "Project path used is : ${PROJECTROOT}/${PROJECTNAME}.pro"
source set_brew_env.sh


BUILDROOTFOLDER=build-${PROJECTNAME}-package

if [ -d ${BUILDROOTFOLDER}/${MODE} ]; then
	rm -rf ${BUILDROOTFOLDER}/${MODE}
fi
mkdir -p ${BUILDROOTFOLDER}/${MODE}/debug
mkdir -p ${BUILDROOTFOLDER}/${MODE}/release
echo "===========> building ${PROJECTNAME} project <==========="
pushd ${BUILDROOTFOLDER}/${MODE}/debug
`${QMAKE_PATH}/qmake ${PROJECTROOT}/${PROJECTNAME}.pro -spec ${QMAKE_SPEC} CONFIG+=package_remaken ${QMAKE_MODE} CONFIG+=debug CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd
pushd ${BUILDROOTFOLDER}/${MODE}/release
`${QMAKE_PATH}/qmake ${PROJECTROOT}/${PROJECTNAME}.pro -spec ${QMAKE_SPEC} CONFIG+=package_remaken ${QMAKE_MODE} CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd

