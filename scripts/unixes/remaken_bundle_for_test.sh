#!/bin/bash

source version.sh
version=${VERSION}

REMAKEN_PLATFORM=linux-gcc
REMAKEN_ARCH=x86_64
suffix=_static_deps
BUILDMODES="debug release"

#Bin
PROJECTS="testxpcf xpcf_grpc_client xpcf_grpc_server"
for project in ${PROJECTS}
do
for buildMode in ${BUILDMODES}
do
echo "--------- Project ${project} ---------"
echo "--------------------------------------"
remaken bundle -v --recurse -d ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}/${version}/bin/x86_64/shared/${buildMode} ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}/${version}/packagedependencies.txt -c ${buildMode}
remaken bundle -v --recurse -d ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}${suffix}/${version}/bin/x86_64/shared/${buildMode} ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}${suffix}/${version}/packagedependencies.txt -c ${buildMode}
done
done

#lib
PROJECTS="xpcfGrpcRemotingxpcfSampleComponent"
#PROJECTS="xpcf"
for project in ${PROJECTS}
do
for buildMode in ${BUILDMODES}
do
echo "--------- Project ${project} - ${buildMode} ---------"
echo "-----------------------------------------------------"
remaken bundle -v --recurse -d ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}/${version}/lib/x86_64/shared/${buildMode} ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}/${version}/packagedependencies.txt -c ${buildMode}
remaken bundle -v --recurse -d ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}${suffix}/${version}/lib/x86_64/shared/${buildMode} ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}${suffix}/${version}/packagedependencies.txt -c ${buildMode}

done
done

#project=xpcfSampleRunner
#remaken bundle -v --recurse -d ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}/1.0.0/bin/x86_64/shared/${buildMode} ~/.remaken/packages/${REMAKEN_PLATFORM}/${project}/1.0.0/packagedependencies.txt -c ${buildMode}