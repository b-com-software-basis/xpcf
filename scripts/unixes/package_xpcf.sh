#!/bin/bash

QTVERSION=5.15.2
XPCFROOT=../..

display_usage() { 
	echo "This script builds xpcf and xpcfSampleComponent in shared and static mode with a special deployment option for packaging."
	echo "It also compress the various xpcf packages." 
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

${XPCFROOT}/scripts/unixes/build_remaken_project_package.sh xpcf shared ${XPCFROOT} ${QTVERSION}
${XPCFROOT}/scripts/unixes/build_remaken_project_package.sh xpcf static ${XPCFROOT} ${QTVERSION}
${XPCFROOT}/scripts/unixes/build_remaken_project_package.sh xpcfSampleComponent shared ${XPCFROOT}/samples/sample_component ${QTVERSION}

${XPCFROOT}/scripts/unixes/make_remaken_package.sh xpcf 2.5.2
${XPCFROOT}/scripts/unixes/make_remaken_package.sh xpcfSampleComponent 2.5.2
