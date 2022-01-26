#!/bin/bash

PKGNAMES="grpc grpc-openssl"
PKGVERSION=1.37.1
BUILDMODES="debug release"
LINKMODES="static shared"


for pkgName in ${PKGNAMES}
do
for linkMode in ${LINKMODES}
do
for buildMode in ${BUILDMODES}
do
BUILDFOLDER=$HOME/.local/${linkMode}/${buildMode}
if [ -d ${BUILDFOLDER}/${pkgName} ]
then
echo "Packaging ${pkgName}-${PKGVERSION} from folder ${BUILDFOLDER}/${pkgName}"
pushd ${BUILDFOLDER}
zip -r -y ${pkgName}_${PKGVERSION}_x86_64_${linkMode}_${buildMode}.zip ${pkgName}
popd
fi
done
done
done
