#!/bin/bash
QTVERSION=5.15.2
XPCFROOT=../..

display_usage() { 
	echo "This script builds xpcf in shared and static mode."
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

${XPCFROOT}/scripts/unixes/build_remaken_project.sh xpcf shared ${XPCFROOT} ${QTVERSION}
${XPCFROOT}/scripts/unixes/build_remaken_project.sh xpcf static ${XPCFROOT} ${QTVERSION}
