#!/bin/bash
QTVERSION=5.15.2
XPCFROOT=..

display_usage() { 
	echo "This script builds xpcf in shared and static mode, xpcf_grpc_gen and testxpcf applications."
    echo "It can receive two optional arguments." 
	echo -e "\nUsage: \$0 [path to xpcf project root | default='${XPCFROOT}'] [Qt kit version to use | default='${QTVERSION}'] \n" 
} 

if [  $# -lt 1 ] 
then 
    display_usage
    exit 1
fi 

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

if [ ! -d ${XPCFROOT} ]; then
	echo "XPCF project root path '${XPCFROOT}' doesn't exist"
	exit 2
fi
echo "XPCF project root path used is : ${XPCFROOT}"
${XPCFROOT}/scripts/build_xpcf_shared.sh ${XPCFROOT} ${QTVERSION}
${XPCFROOT}/scripts/build_testxpcf.sh ${XPCFROOT} ${QTVERSION}
${XPCFROOT}/scripts/build_xpcf_static.sh ${XPCFROOT} ${QTVERSION}
${XPCFROOT}/scripts/build_xpcf_grpc_gen.sh ${XPCFROOT} ${QTVERSION}
