#!/bin/bash

PKGNAME=grpc
PKGVERSION=1.37.1
BUILDMODES="debug release"
LINKMODES="static shared"
for linkMode in ${LINKMODES}
do
for buildMode in ${BUILDMODES}
do
BUILDFOLDER=$HOME/.local/${linkMode}/${buildMode}
if [ -d ${BUILDFOLDER}/${PKGNAME} ]
then
echo "Packaging ${PKGNAME}-${PKGVERSION} from folder ${BUILDFOLDER}/${PKGNAME}"
pushd ${BUILDFOLDER}
zip -r ${PKGNAME}_${PKGVERSION}_x86_64_${linkMode}_${buildMode}.zip ${PKGNAME}
popd
fi
done
done
