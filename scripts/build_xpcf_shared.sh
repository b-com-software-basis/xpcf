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
mkdir -p build/xpcf_shared
echo "===========> building XPCF shared <==========="
pushd build/xpcf_shared
`${QMAKE_PATH}/qmake ../../${XPCFROOT}/xpcf.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
rm *
`${QMAKE_PATH}/qmake ../../${XPCFROOT}/xpcf.pro -spec linux-g++ CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd

