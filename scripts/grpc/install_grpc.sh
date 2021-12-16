#!/bin/bash

GRPCVERSION=1.37.1
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    INSTALL_TARGET=linux-gcc
elif [[ "$OSTYPE" == "darwin"* ]]; then
    INSTALL_TARGET=mac-clang
fi

# default OpenSSL mode = module (borringSSl) - alternative = package (openssl 1.1.1k)
OPENSSL_DEFAULT_MODE=module

display_usage() { 
	echo "This script builds grpc in static and shared mode."
    echo "It can receive one optional arguments." 
	echo -e "\nUsage: \$0 [OpenSSL version - default='module' (internal boringssl) or 'package' with a conan openssl 1.1.1k version] \n" 
} 

# check whether user had supplied -h or --help . If yes display usage 
if [[ ( $1 == "--help") ||  $1 == "-h" ]] 
then 
    display_usage
    exit 0
fi 

if [ $# -eq 1 ]; then
	OPENSSL_DEFAULT_MODE=$1
fi
echo "GRPC Openssl default mode is : ${OPENSSL_DEFAULT_MODE}"

if [[ "${OPENSSL_DEFAULT_MODE}" == "package" ]]; then
    cp bcom-debug-grpc++-openssl.pc $HOME/.local/static/debug/grpc/${GRPCVERSION}/bcom-debug-grpc++.pc
    cp bcom-debug-grpc++-openssl.pc $HOME/.local/shared/debug/grpc/${GRPCVERSION}/bcom-debug-grpc++.pc
    cp bcom-grpc++-openssl.pc $HOME/.local/shared/release/grpc/${GRPCVERSION}/bcom-grpc++.pc
    cp bcom-grpc++-openssl.pc $HOME/.local/static/release/grpc/${GRPCVERSION}/bcom-grpc++.pc
else
    cp bcom-debug-grpc++.pc $HOME/.local/static/debug/grpc/${GRPCVERSION}/
    cp bcom-debug-grpc++.pc $HOME/.local/shared/debug/grpc/${GRPCVERSION}/
    cp bcom-grpc++.pc $HOME/.local/shared/release/grpc/${GRPCVERSION}/
    cp bcom-grpc++.pc $HOME/.local/static/release/grpc/${GRPCVERSION}/
fi

cp -r $HOME/.local/static/debug/grpc/${GRPCVERSION} $HOME/.remaken/packages/${INSTALL_TARGET}/grpc/
cp -r $HOME/.local/static/release/grpc/${GRPCVERSION} $HOME/.remaken/packages/${INSTALL_TARGET}/grpc/
cp -r $HOME/.local/shared/debug/grpc/${GRPCVERSION} $HOME/.remaken/packages/${INSTALL_TARGET}/grpc/
cp -r $HOME/.local/shared/release/grpc/${GRPCVERSION} $HOME/.remaken/packages/${INSTALL_TARGET}/grpc/
