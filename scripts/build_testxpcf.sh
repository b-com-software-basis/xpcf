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
mkdir -p build/testxpcf
mkdir -p build/samplecomponent

echo "===========> building XPCF sample component <==========="
pushd build/samplecomponent
`${QMAKE_PATH}/qmake ../../${XPCFROOT}/samples/sample_component/xpcfSampleComponent.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
rm *
`${QMAKE_PATH}/qmake ../../${XPCFROOT}/samples/sample_component/xpcfSampleComponent.pro -spec linux-g++ CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd
echo "===========> building testxpcf <==========="
pushd build/testxpcf
`${QMAKE_PATH}/qmake ../../${XPCFROOT}/test/testxpcf.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
rm *
`${QMAKE_PATH}/qmake ../../${XPCFROOT}/test/testxpcf.pro -spec linux-g++ CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd


