#!/bin/bash
TARGET_LANG="csharp"
DESTFOLDER="swig-xpcf-cxx"
DESTSAMPLEFOLDER="swig-xpcf-sample-cxx"

display_usage() { 
	echo "This script cleans swig csharp for xpcf."
} 
# check whether user had supplied -h or --help . If yes display usage
if [[ ( $1 == "--help") ||  $1 == "-h" ]]
then
    display_usage
    exit 0
fi

if [ -d ${TARGET_LANG}-android ]; then
    echo "Removing folder ${TARGET_LANG}-android"
    rm -rf ${TARGET_LANG}-android
fi

if [ -d ${TARGET_LANG}-sample ]; then
    echo "Removing folder ${TARGET_LANG}-sample"
    rm -rf ${TARGET_LANG}-sample
fi

if [ -d ${TARGET_LANG} ]; then
    echo "Removing folder ${TARGET_LANG}"
    rm -rf ${TARGET_LANG}
fi

if [ -d ${DESTFOLDER} ]; then
    echo "Removing folder ${DESTFOLDER}"
    rm -rf ${DESTFOLDER}
fi

if [ -d ${DESTSAMPLEFOLDER} ]; then
    echo "Removing folder ${DESTSAMPLEFOLDER}"
    rm -r ${DESTSAMPLEFOLDER}
fi

if [ -d test/XPCF ]; then
    echo "Suppress csharp interfaces in test/XPCF"
    rm -rf test/XPCF
fi

if [ -d test/bin ]; then
    echo "Suppress csharp binaries in test/bin"
    rm -rf test/bin
fi

if [ -d test/obj ]; then
    echo "Suppress csharp objects in test/obj"
    rm -rf test/obj
fi

if [ -d build-swig-xpcf-sample ]; then
    echo "Suppress folder build-swig-xpcf-sample"
    rm -rf build-swig-xpcf-sample
fi
