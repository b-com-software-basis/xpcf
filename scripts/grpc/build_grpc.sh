#!/bin/bash

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    echo "installing cmake"
    sudo apt install -y cmake
    echo "installing build-essential autoconf libtool pkg-config"
    sudo apt install -y build-essential autoconf libtool pkg-config
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

GRPCVERSION=1.37.1
REMAKENGRPCROOT=grpc/${GRPCVERSION}
SSL_BUILD=boringssl

if [ $# -eq 1 ]; then
	SSL_BUILD=$1
fi
SSL_MODE=module
if [[ "${SSL_BUILD}" == "openssl" ]]; then
	SSL_MODE=package
    REMAKENGRPCROOT=grpc-${SSL_BUILD}/${GRPCVERSION}
fi
echo "GRPC ssl mode is : ${SSL_MODE}"

git clone --recurse-submodules -b v${GRPCVERSION} https://github.com/grpc/grpc

function buildGrpcShared {
    LINKMODE=$(echo "$1" | tr '[:upper:]' '[:lower:]')
    if [[ "${SSL_MODE}" == "package" ]]; then
        conan install ./conanfile_openssl.txt -s arch=x86_64 -s compiler.cppstd=17 -s build_type=$1 --build=missing -if ./_build_openssl
    fi
    BUILDFOLDER=.build/build_shared_${SSL_BUILD}/$1
    INSTFOLDER=$HOME/.local/shared/${LINKMODE}/${REMAKENGRPCROOT}/install
    pushd grpc
    mkdir -p $INSTFOLDER
#    export PATH="$PATH:$INSTFOLDER/bin"
    mkdir -p $BUILDFOLDER
    pushd  $BUILDFOLDER
    
    cmake -DCMAKE_CXX_STANDARD=17 \
    	  -DgRPC_INSTALL=ON \
	  -DgRPC_BUILD_TESTS=OFF \
	  -DCMAKE_INSTALL_PREFIX=$INSTFOLDER \
	  -DBUILD_SHARED_LIBS=ON \
	  -DgRPC_INSTALL=ON                \
	  -DABSL_ENABLE_INSTALL=ON	\
	  -DCMAKE_BUILD_TYPE=$1       \
	  -DgRPC_ABSL_PROVIDER=module     \
	  -DgRPC_CARES_PROVIDER=module    \
	  -DgRPC_PROTOBUF_PROVIDER=module \
	  -DgRPC_RE2_PROVIDER=module      \
	  -DgRPC_SSL_PROVIDER=${SSL_MODE}      \
	  -DgRPC_ZLIB_PROVIDER=module \
	  ../../../..

    make
    make install
    popd
    popd
    pushd $HOME/.local/shared/${LINKMODE}/${REMAKENGRPCROOT}
    mkdir -p interfaces
    mkdir -p bin/x86_64/shared/${LINKMODE}
    mkdir -p lib/x86_64/shared/${LINKMODE}
    rm -rf install/lib/cmake
    rm -rf install/lib/pkgconfig
    mv -f install/bin/* bin/x86_64/shared/${LINKMODE}/
    mv -f install/lib/* lib/x86_64/shared/${LINKMODE}/
    mv -f install/include/* interfaces
    rm -rf install
    popd
#    rm -rf $BUILDFOLDER
}


function buildGrpcStatic {
    LINKMODE=$(echo "$1" | tr '[:upper:]' '[:lower:]') 
    if [[ "${SSL_MODE}" == "package" ]]; then
        conan install ./conanfile_openssl.txt -s arch=x86_64 -s compiler.cppstd=17 -s build_type=$1 --build=missing -if ./_build_openssl
    fi
    BUILDFOLDER=.build/build_static_${SSL_BUILD}/$1
    INSTFOLDER=$HOME/.local/static/${LINKMODE}/${REMAKENGRPCROOT}/install
    pushd grpc
    mkdir -p $INSTFOLDER
#    export PATH="$PATH:$INSTFOLDER/bin"
    mkdir -p $BUILDFOLDER
    pushd  $BUILDFOLDER
    cmake -DCMAKE_CXX_STANDARD=17 \
    	  -DgRPC_INSTALL=ON \
	  -DgRPC_BUILD_TESTS=OFF \
	  -DCMAKE_INSTALL_PREFIX=$INSTFOLDER \
	  -DBUILD_SHARED_LIBS=OFF \
	  -DgRPC_INSTALL=ON                \
	  -DABSL_ENABLE_INSTALL=ON	\
	  -DCMAKE_BUILD_TYPE=$1       \
	  -DgRPC_ABSL_PROVIDER=module     \
	  -DgRPC_CARES_PROVIDER=module    \
	  -DgRPC_PROTOBUF_PROVIDER=module \
	  -DgRPC_RE2_PROVIDER=module      \
	  -DgRPC_SSL_PROVIDER=${SSL_MODE}      \
	  -DgRPC_ZLIB_PROVIDER=module \
	  ../../../..

    make
    make install
    popd
    popd
    pushd $HOME/.local/static/${LINKMODE}/${REMAKENGRPCROOT}
    mkdir -p interfaces
    mkdir -p bin/x86_64/static/${LINKMODE}
    mkdir -p lib/x86_64/static/${LINKMODE}
    rm -rf install/lib/cmake
    rm -rf install/lib/pkgconfig
    mv -f install/bin/* bin/x86_64/static/${LINKMODE}/
    mv -f install/lib/* lib/x86_64/static/${LINKMODE}/
    mv -f install/include/* interfaces
    rm -rf install
    popd
    rm -rf $BUILDFOLDER
}


buildGrpcShared Release
buildGrpcShared Debug
buildGrpcStatic Release
buildGrpcStatic Debug

# ajout define WITHREMOTING dans sample.pro
# ajout openssl.pc
# ajout var PKG_CONFIG_PATH dans SOLARNATURAL build pointant sur racine package grpc pour autres .pc (voir pour aggrégat des .pc, pas évident avec link static cares
# copie libcares.a dans shared/debug
