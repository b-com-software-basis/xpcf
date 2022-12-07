#!/bin/bash
QTVERSION=5.15.2
XPCFROOT=../..

# default linux values
QMAKEPATH=$HOME/Qt/${QTVERSION}/gcc_64/bin
if [[ "$OSTYPE" == "darwin"* ]]; then
	# overload for mac values
	QMAKE_PATH=/Application/Qt/${QTVERSION}/clang_64/bin
fi

display_usage() { 
	echo "This script builds xpcf in shared and static mode, xpcf_grpc_gen and testxpcf applications."
    echo "It can receive three optional arguments." 
	echo -e "\nUsage: \$0 [path to xpcf project root | default='${XPCFROOT}'] [Qt kit version to use | default='${QTVERSION}'] [path to qmake | default='${QMAKEPATH}'] \n" 
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
if [ $# -ge 2 ]; then
	QTVERSION=$2
fi
if [ $# -eq 3 ]; then
	QMAKEPATH=$3
fi

if [ ! -d ${XPCFROOT}/scripts/unixes/build-xpcf ]; then
	echo "${XPCFROOT}/scripts/unixes/build-xpcf path doesn't exist"
	echo "Running build_xpcf.sh"
	${XPCFROOT}/scripts/unixes/build_xpcf.sh ${XPCFROOT} ${QTVERSION} ${QMAKEPATH}
fi

${XPCFROOT}/scripts/unixes/build_remaken_project.sh xpcf_grpc_client shared ${XPCFROOT}/tools ${QTVERSION} ${QMAKEPATH}
${XPCFROOT}/scripts/unixes/build_remaken_project.sh xpcf_grpc_server shared ${XPCFROOT}/tools ${QTVERSION} ${QMAKEPATH}
${XPCFROOT}/scripts/unixes/build_remaken_project.sh xpcf_grpc_gen static ${XPCFROOT}/tools/generators/grpc ${QTVERSION} ${QMAKEPATH}
