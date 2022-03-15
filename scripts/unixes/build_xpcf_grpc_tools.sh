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
	XPCFROOT=$1
fi
if [ $# -eq 2 ]; then
	QTVERSION=$2
fi
# default linux values
if [[ "$OSTYPE" == "darwin"* ]]; then
# overload for mac values
	export PKG_CONFIG_PATH=/usr/local/opt/openssl@1.1/lib/pkgconfig:/usr/local/lib/pkgconfig
fi

if [ ! -d ${XPCFROOT} ]; then
	echo "XPCF project root path '${XPCFROOT}' doesn't exist"
	exit 2
fi
echo "XPCF project root path used is : ${XPCFROOT}"
${XPCFROOT}/scripts/unixes/build_xpcf_shared.sh ${XPCFROOT} ${QTVERSION}
${XPCFROOT}/scripts/unixes/build_xpcf_grpc_client.sh ${XPCFROOT} ${QTVERSION}
${XPCFROOT}/scripts/unixes/build_xpcf_grpc_server.sh ${XPCFROOT} ${QTVERSION}
${XPCFROOT}/scripts/unixes/build_xpcf_static.sh ${XPCFROOT} ${QTVERSION}
${XPCFROOT}/scripts/unixes/build_xpcf_grpc_gen.sh ${XPCFROOT} ${QTVERSION}
