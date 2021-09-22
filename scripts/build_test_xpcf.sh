#!/bin/bash
QMAKE_PATH=$HOME/Qt/5.15.2/gcc_64/bin
XPCFROOT=..
if [ $# -eq 1 ]; then
	XPCFROOT=$1
fi
if [ ! -d ${XPCFROOT} ]; then
	echo "XPCF project root path '${XPCFROOT}' doesn't exist"
	exit 2
fi
echo "XPCF project root path used is : ${XPCFROOT}"
${XPCFROOT}/scripts/build_xpcf_shared.sh ${XPCFROOT}
${XPCFROOT}/scripts/build_testxpcf.sh ${XPCFROOT}
${XPCFROOT}/scripts/build_xpcf_static.sh ${XPCFROOT}
${XPCFROOT}/scripts/build_xpcf_grpc_gen.sh ${XPCFROOT}
