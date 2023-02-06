#!/bin/bash


display_usage() { 
	echo "This script builds grpc in static and shared mode for Android."
        echo "It can receive one required argument and one optional arguments." 
	echo -e "\nUsage: $0 [required path to Android NDK] [optionnal - build with SSL - default='boringssl' (internal boringssl) or 'openssl' with a conan openssl 1.1.1k version] \n" 
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
SSL_MODE=module
ANDROID_PLATFORM=21

GRPC_TOOLS_PATH=$PWD/tools
if [ ! -d tools ]; then
    mkdir tools
fi

# Get NDK to build GRPC. We recommand the version r21e which the default installed version.
if [ $# -ge 1 ]; then
	ANDROID_NDK=$1
	echo "Toolchain : ${ANDROID_NDK}/build/cmake/android.toolchain.cmake"
else
    ANDROID_NDK=$GRPC_TOOLS_PATH/android-ndk-r21e/
    if [[ ! -d $ANDROID_NDK ]]; then
        pushd $GRPC_TOOLS_PATH
        curl -sSL https://dl.google.com/android/repository/android-ndk-r21e-linux-x86_64.zip -o android-ndk-r21e.zip
        unzip -oq android-ndk-r21e.zip
        rm -f android-ndk-r21e.zip
        echo "Toolchain : ${ANDROID_NDK}/build/cmake/android.toolchain.cmake"
        popd
    fi
fi

# Install tools to build grpc on Linux for the target Android.
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    echo "installing cmake"
    sudo apt install -y cmake
    echo "installing build-essential autoconf libtool pkg-config"
    sudo apt install -y build-essential autoconf libtool pkg-config
fi

# Install boringssl submodule and patch it to link with crypto
git clone --recurse-submodules -b v${GRPCVERSION} https://github.com/grpc/grpc
SSL_SUBMODULE_PATH=grpc/third_party/boringssl-with-bazel
pushd $SSL_SUBMODULE_PATH
git apply ../../../patch/link_ssl_crypto.patch
popd

# Get tools to generate GRPC stubs. Tools come from previous build of grpc on Linux in static mode
pushd tools
if [[ -f tools/protoc && -f tools/grpc_cpp_plugin ]]; then
    echo "Tools to generate grpc already present"
else
    curl -sSL https://github.com/b-com-software-basis/thirdparties-binaries/releases/download/android_grpc_tools/$GRPCVERSION/android_grpc_tools.zip -o android_grpc_tools.zip
    unzip -oq android_grpc_tools.zip
    rm -f android_grpc_tools.zip
fi
popd

function buildGrpcShared {
    LINKMODE=$(echo "$1" | tr '[:upper:]' '[:lower:]')
    if [[ "${SSL_MODE}" == "package" ]]; then
        conan install ./conanfile_openssl.txt -s arch=armv8 -s os=Android -s os.api_level=21 -s compiler=clang -s compiler.version=9 -s compiler.cppstd=17 -s build_type=$1 --build=missing -if ./_build_openssl
    fi
    BUILDFOLDER=.build/build_shared_${SSL_BUILD}/$1
    INSTFOLDER=$HOME/.local/android/shared/${LINKMODE}/${REMAKENGRPCROOT}/install
    pushd grpc
    mkdir -p $INSTFOLDER
#    export PATH="$PATH:$INSTFOLDER/bin"
    mkdir -p $BUILDFOLDER
    pushd  $BUILDFOLDER
    cmake -DCMAKE_CXX_STANDARD=17 \
    	  -DgRPC_INSTALL=ON \
	  -DgRPC_BUILD_TESTS=OFF \
	  -DCMAKE_INSTALL_PREFIX=$INSTFOLDER \
  	  -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
      -DCMAKE_PROGRAM_PATH=$GRPC_TOOLS_PATH \
      -DCMAKE_C_FLAGS="-fuse-ld=lld" \
      -DCMAKE_CXX_FLAGS="-fuse-ld=lld" \
	  -DANDROID_ABI=arm64-v8a \
	  -DANDROID_PLATFORM=android-$ANDROID_PLATFORM \
	  -DANDROID_STL=c++_static \
	  -DBUILD_SHARED_LIBS=ON \
	  -DgRPC_INSTALL=ON \
	  -DABSL_ENABLE_INSTALL=ON \
	  -DCMAKE_BUILD_TYPE=$1 \
	  -DgRPC_ABSL_PROVIDER=module \
	  -DgRPC_CARES_PROVIDER=module \
	  -DgRPC_PROTOBUF_PROVIDER=module \
	  -DgRPC_RE2_PROVIDER=module \
	  -DgRPC_SSL_PROVIDER=${SSL_MODE} \
	  -DgRPC_ZLIB_PROVIDER=module \
	  ../../../..
    make
    make install
    popd
    popd
    pushd $HOME/.local/android/shared/${LINKMODE}/${REMAKENGRPCROOT}
    mkdir -p interfaces
    mkdir -p bin/arm64_v8a/shared/${LINKMODE}
    mkdir -p lib/arm64_v8a/shared/${LINKMODE}
    rm -rf install/lib/cmake
    rm -rf install/lib/pkgconfig
    mv -f install/bin/* bin/arm64_v8a/shared/${LINKMODE}/
    mv -f install/lib/* lib/arm64_v8a/shared/${LINKMODE}/
    mv -f install/include/* interfaces
    rm -rf install
    popd
    rm -rf $BUILDFOLDER
}


function buildGrpcStatic {
    LINKMODE=$(echo "$1" | tr '[:upper:]' '[:lower:]') 
    if [[ "${SSL_MODE}" == "package" ]]; then
        conan install ./conanfile_openssl.txt -s arch=armv8 -s os=Android -s os.api_level=21 -s compiler=clang -s compiler.version=8 -s compiler.cppstd=17 -s build_type=$1 --build=missing -if ./_build_openssl
    fi
    BUILDFOLDER=.build/build_static_${SSL_BUILD}/$1
    INSTFOLDER=$HOME/.local/android/static/${LINKMODE}/${REMAKENGRPCROOT}/install
    pushd grpc
    mkdir -p $INSTFOLDER
#    export PATH="$PATH:$INSTFOLDER/bin"
    mkdir -p $BUILDFOLDER
    pushd  $BUILDFOLDER
    cmake -DCMAKE_CXX_STANDARD=17 \
	  -DgRPC_INSTALL=ON \
	  -DgRPC_BUILD_TESTS=OFF \
	  -DCMAKE_INSTALL_PREFIX=$INSTFOLDER \
	  -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
      -DCMAKE_PROGRAM_PATH=$GRPC_TOOLS_PATH \
	  -DANDROID_ABI=arm64-v8a \
	  -DANDROID_PLATFORM=android-$ANDROID_PLATFORM \
      -DCMAKE_C_FLAGS="-fuse-ld=lld" \
      -DCMAKE_CXX_FLAGS="-fuse-ld=lld" \
	  -DANDROID_STL=c++_static \
	  -DBUILD_SHARED_LIBS=OFF \
	  -DgRPC_INSTALL=ON \
	  -DABSL_ENABLE_INSTALL=ON \
	  -DCMAKE_BUILD_TYPE=$1 \
	  -DgRPC_ABSL_PROVIDER=module \
	  -DgRPC_CARES_PROVIDER=module \
	  -DgRPC_PROTOBUF_PROVIDER=module \
	  -DgRPC_RE2_PROVIDER=module \
	  -DgRPC_SSL_PROVIDER=${SSL_MODE} \
	  -DgRPC_ZLIB_PROVIDER=module \
	  ../../../..

    make
    make install
    popd
    popd
    pushd $HOME/.local/android/static/${LINKMODE}/${REMAKENGRPCROOT}
    mkdir -p interfaces
    mkdir -p bin/arm64_v8a/static/${LINKMODE}
    mkdir -p lib/arm64_v8a/static/${LINKMODE}
    rm -rf install/lib/cmake
    rm -rf install/lib/pkgconfig
    mv -f install/bin/* bin/arm64_v8a/static/${LINKMODE}/
    mv -f install/lib/* lib/arm64_v8a/static/${LINKMODE}/
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
