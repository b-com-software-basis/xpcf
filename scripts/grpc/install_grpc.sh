#!/bin/bash

GRPCVERSION=1.37.1
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    INSTALL_TARGET=linux-gcc
elif [[ "$OSTYPE" == "darwin"* ]]; then
    INSTALL_TARGET=mac-clang
fi
cp bcom-debug-grpc++.pc .local/static/debug/grpc/${GRPCVERSION}/
cp bcom-debug-grpc++.pc .local/shared/debug/grpc/${GRPCVERSION}/
cp bcom-grpc++.pc .local/shared/release/grpc/${GRPCVERSION}/
cp bcom-grpc++.pc .local/static/release/grpc/${GRPCVERSION}/
cp -r .local/static/debug/grpc/${GRPCVERSION} .remaken/packages/${INSTALL_TARGET}/grpc/
cp -r .local/static/release/grpc/${GRPCVERSION} .remaken/packages/${INSTALL_TARGET}/grpc/
cp -r .local/shared/debug/grpc/${GRPCVERSION} .remaken/packages/${INSTALL_TARGET}/grpc/
cp -r .local/shared/release/grpc/${GRPCVERSION} .remaken/packages/${INSTALL_TARGET}/grpc/
