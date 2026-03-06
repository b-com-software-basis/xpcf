#!/bin/bash

source version.sh
version=${VERSION}

REMAKEN_PLATFORM=linux-gcc
REMAKEN_ARCH=x86_64
suffix=_static_deps
BUILDMODES="debug release"

PROJECTS="xpcf testxpcf xpcf_grpc_client xpcf_grpc_server xpcfGrpcRemotingxpcfSampleComponent"
for project in ${PROJECTS}
do
for buildMode in ${BUILDMODES}
do
echo "--------- Project ${project} ---------"
echo "--------------------------------------"

remaken install  ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}/${version}/packagedependencies.txt -v -c ${buildMode} --condition "XPCF_SHARED_DEPS=false" --condition "XPCF_STATIC_DEPS=true"
remaken install  ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}/${version}/packagedependencies.txt -v -c ${buildMode} --condition "XPCF_SHARED_DEPS=true" --condition "XPCF_STATIC_DEPS=false"
remaken install  ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}${suffix}/${version}/packagedependencies.txt -v -c ${buildMode} --condition "XPCF_SHARED_DEPS=false" --condition "XPCF_STATIC_DEPS=true"
remaken install  ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}${suffix}/${version}/packagedependencies.txt -v -c ${buildMode} --condition "XPCF_SHARED_DEPS=true" --condition "XPCF_STATIC_DEPS=false"
done
done