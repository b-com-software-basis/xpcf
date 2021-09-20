#!/bin/bash

PKGNAME=xpcf
PKGVERSION=2.5.0
BUILDMODES="debug release"
LINKMODES="static shared"
for linkMode in ${LINKMODES}
do
for buildMode in ${BUILDMODES}
do
BUILDFOLDER=$HOME/.remaken/packages/linux-gcc/${linkMode}/${buildMode}
if [ -d ${BUILDFOLDER}/${PKGNAME} ]
then
echo "Packaging ${PKGNAME}-${PKGVERSION} from folder ${BUILDFOLDER}/${PKGNAME}"
pushd ${BUILDFOLDER}
zip -r ${PKGNAME}_${PKGVERSION}_x86_64_${linkMode}_${buildMode}.zip ${PKGNAME}
popd
fi
done
done
