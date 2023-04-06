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

XPCF_GRPC_GEN_APPLICATION=xpcf_grpc_gen

if [[ "$OSTYPE" == "darwin"* ]]; then
	# overload for mac values
	XPCF_GRPC_GEN_APPLICATION=xpcf_grpc_gen.app/Contents/MacOS/xpcf_grpc_gen
fi

if [ ! -d ${XPCFROOT}/scripts/unixes/build-xpcf_grpc_gen/static/release ]; then
	echo "${XPCFROOT}/scripts/unixes/build-xpcf_grpc_gen/static/release path doesn't exist: please run build_xpcf_grpc_tools.sh before building test sample"
	exit 2
fi

${XPCFROOT}/scripts/unixes/build-xpcf_grpc_gen/static/release/${XPCF_GRPC_GEN_APPLICATION} -n xpcfSampleComponent -v 2.6.3 -r @github -u https://github.com/b-com-software-basis/xpcf/releases/releases/download --std c++1z --database_dir ../../samples/sample_component/ --remove_comments_in_macro -o ${XPCFROOT}/scripts/unixes/build-grpc_gen_folder_sampleComponent -g protobuf -i workspace/github/xpcf/samples/sample_component
${XPCFROOT}/scripts/unixes/build_remaken_project.sh xpcfGrpcRemotingxpcfSampleComponent shared ${XPCFROOT}/scripts/unixes/build-grpc_gen_folder_sampleComponent ${QMAKEPATH}
