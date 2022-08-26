#!/bin/bash
QTVERSION=5.15.2
XPCFROOT=../..

display_usage() { 
	echo "This script builds xpcf in shared and static mode, xpcf_grpc_gen and testxpcf applications."
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
	QMAKEPATH=$1
fi
if [ $# -ge 2 ]; then
	XPCFROOT=$2
fi
if [ $# -eq 3 ]; then
	QTVERSION=$3
fi

if [ ! -d ${XPCFROOT}/scripts/unixes/build-xpcf ]; then
	echo "${XPCFROOT}/scripts/unixes/build-xpcf path doesn't exist"
	echo "Running build_xpcf.sh"
	${XPCFROOT}/scripts/unixes/build_xpcf.sh ${QMAKEPATH} ${XPCFROOT} ${QTVERSION}
fi

${XPCFROOT}/scripts/unixes/build_remaken_project.sh testxpcf shared ${QMAKEPATH} ${XPCFROOT}/test ${QTVERSION}
${XPCFROOT}/scripts/unixes/build_remaken_project.sh xpcfcli static ${QMAKEPATH} ${XPCFROOT}/tools/cli ${QTVERSION}
