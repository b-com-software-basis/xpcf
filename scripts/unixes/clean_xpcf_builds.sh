#!/bin/bash
XPCFROOT=../..

display_usage() { 
	echo "This script cleans every xpcf builds."
    echo "It can receive one optional arguments." 
	echo -e "\nUsage: \$0 [path to xpcf project root | default='${XPCFROOT}']\n" 
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

if [ ! -d ${XPCFROOT} ]; then
	echo "XPCF project root path '${XPCFROOT}' doesn't exist"
	exit 2
fi
echo "XPCF project root path used is : ${XPCFROOT}"
for i in $(find ${XPCFROOT}/scripts/unixes -type d -name build-\*)
do
	echo "Removing $i folder"
	rm -rf $i
done
