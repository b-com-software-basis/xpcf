#!/bin/bash
QMAKE_PATH=$HOME/Qt/5.15.2/gcc_64/bin
XPCFROOT=..
if [ $# -eq 1 ]; then
	XPCFROOT=$1
fi
if [ ! -d ${XPCFROOT} ]; then
	echo "XPCF project root path '${XPCFROOT}' doesn't exist"
	exit 2
fi
echo "XPCF project root path used is : ${XPCFROOT}"
mkdir -p build/xpcf_grpc_gen
echo "===========> building cppast <==========="
pushd ../libs
./build_cppast.sh
popd
echo "===========> building xpcf_grpc_gen <==========="
pushd build/xpcf_grpc_gen
`${QMAKE_PATH}/qmake ../../${XPCFROOT}/tools/generators/grpc/xpcf_grpc_gen.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
rm *
`${QMAKE_PATH}/qmake ../../${XPCFROOT}/tools/generators/grpc/xpcf_grpc_gen.pro -spec linux-g++ CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd

