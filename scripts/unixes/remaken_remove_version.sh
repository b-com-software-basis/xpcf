#!/bin/bash


source version.sh
version=${VERSION}

REMAKEN_PLATFORM=linux-gcc
REMAKEN_ARCH=x86_64
suffix=_static_deps

clear

PROJECTS="xpcf testxpcf xpcf_grpc_client xpcf_grpc_server xpcf_grpc_gen xpcfGrpcRemotingxpcfSampleComponent xpcfSampleRunner"
for project in ${PROJECTS}
do
echo "--------- Project ${project} ---------"
echo "--------------------------------------"./bu
rm -rf ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}/${version}
rm -rf ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}${suffix}/${version}
done

#project=xpcfSampleRunner
#rm -rf ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}/1.0.0