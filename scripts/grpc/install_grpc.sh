#!/bin/bash

PROJECT=grpc
GRPCVERSION=1.37.1

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    INSTALL_TARGET=linux-gcc
elif [[ "$OSTYPE" == "darwin"* ]]; then
    INSTALL_TARGET=mac-clang
fi

display_usage() { 
	echo "This script builds grpc in static and shared mode."
    echo "It can receive one optional arguments." 
	echo -e "\nUsage: \$0 [build with SSL - default='boringssl' (internal boringssl) or 'openssl' with a conan openssl 1.1.1k version] \n" 
} 

# check whether user had supplied -h or --help . If yes display usage 
if [[ ( $1 == "--help") ||  $1 == "-h" ]] 
then 
    display_usage
    exit 0
fi 

# default parameter value - with boringssl
SSL_MODE=boringssl
if [ $# -eq 1 ]; then
	SSL_MODE=$1
fi
if [[ "${SSL_MODE}" == "openssl" ]]; then
    PROJECT=grpc-${SSL_MODE}
fi
echo "GRPC ssl mode is : ${SSL_MODE}"

if [[ "${SSL_MODE}" == "openssl" ]]; then
    cp bcom-debug-grpc++-openssl.pc $HOME/.local/static/debug/${PROJECT}/${GRPCVERSION}/bcom-debug-grpc++.pc
    cp bcom-debug-grpc++-openssl.pc $HOME/.local/shared/debug/${PROJECT}/${GRPCVERSION}/bcom-debug-grpc++.pc
    cp bcom-grpc++-openssl.pc $HOME/.local/shared/release/${PROJECT}/${GRPCVERSION}/bcom-grpc++.pc
    cp bcom-grpc++-openssl.pc $HOME/.local/static/release/${PROJECT}/${GRPCVERSION}/bcom-grpc++.pc
else
    cp bcom-debug-grpc++.pc $HOME/.local/static/debug/${PROJECT}/${GRPCVERSION}/
    cp bcom-debug-grpc++.pc $HOME/.local/shared/debug/${PROJECT}/${GRPCVERSION}/
    cp bcom-grpc++.pc $HOME/.local/shared/release/${PROJECT}/${GRPCVERSION}/
    cp bcom-grpc++.pc $HOME/.local/static/release/${PROJECT}/${GRPCVERSION}/
fi

mkdir -p $HOME/.remaken/packages/${INSTALL_TARGET}/${PROJECT}
cp -r $HOME/.local/static/debug/${PROJECT}/${GRPCVERSION} $HOME/.remaken/packages/${INSTALL_TARGET}/${PROJECT}/
cp -r $HOME/.local/static/release/${PROJECT}/${GRPCVERSION} $HOME/.remaken/packages/${INSTALL_TARGET}/${PROJECT}/
cp -r $HOME/.local/shared/debug/${PROJECT}/${GRPCVERSION} $HOME/.remaken/packages/${INSTALL_TARGET}/${PROJECT}/
cp -r $HOME/.local/shared/release/${PROJECT}/${GRPCVERSION} $HOME/.remaken/packages/${INSTALL_TARGET}/${PROJECT}/
