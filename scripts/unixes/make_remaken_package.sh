#!/bin/bash


display_usage() { 
	echo "This script compress a remaken built package in order to deploy the compressed result on a remote."
    echo "It expects two arguments." 
	echo -e "\nUsage: \$0 [remaken package name] [remaken package version] \n" 
} 

if [  $# -lt 2 ] 
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

PKGNAME=$1
PKGVERSION=$2

REMAKEN_PLATFORM=linux-gcc
if [[ "$OSTYPE" == "darwin"* ]]; then
# overload for mac values
	REMAKEN_PLATFORM=mac-clang
fi

BUILDMODES="debug release"
LINKMODES="static shared"
for linkMode in ${LINKMODES} 
do
    for buildMode in ${BUILDMODES} 
    do
        BUILDFOLDER=$HOME/.remaken/packages/${REMAKEN_PLATFORM}/${PKGNAME}/${PKGVERSION}
        if [ -d ${BUILDFOLDER} -a -d ${BUILDFOLDER}/interfaces -a -d ${BUILDFOLDER}/lib/x86_64/${linkMode}/${buildMode} ] 
        then
            PKGROOT=${PKGNAME}/${PKGVERSION}
            mkdir -p ${PKGROOT}
            # Copy interfaces folder
            cp -R ${BUILDFOLDER}/interfaces ${PKGROOT}
            # Copy lib folder
            mkdir -p ${PKGROOT}/lib/x86_64/${linkMode}
            cp -R ${BUILDFOLDER}/lib/x86_64/${linkMode}/${buildMode} ${PKGROOT}/lib/x86_64/${linkMode}
            if [ -d ${BUILDFOLDER}/wizards ];
            then
                # Copy wizards folder
                cp -R ${BUILDFOLDER}/wizards ${PKGROOT}
            fi
            # Copy other files
            cp ${BUILDFOLDER}/${PKGNAME}-${PKGVERSION}_remakeninfo.txt ${PKGROOT}
            cp ${BUILDFOLDER}/remaken-${PKGNAME}.pc ${PKGROOT}
            if [ "$linkMode" = "static" ]; then
                cp ${BUILDFOLDER}/packagedependencies-static.txt ${PKGROOT}/packagedependencies.txt
            else
                cp ${BUILDFOLDER}/packagedependencies.txt ${PKGROOT} 2>/dev/null
            fi
            zip -q -r -y ${PKGNAME}_${PKGVERSION}_x86_64_${linkMode}_${buildMode}.zip ${PKGNAME}
            # Clean solution
            rm -rf ${PKGNAME}
        fi
    done
done
