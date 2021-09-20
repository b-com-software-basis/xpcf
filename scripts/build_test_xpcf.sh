#!/bin/bash
QMAKE_OPTS=-spec linux-g++ CONFIG+=debug CONFIG+=qml_debug && /usr/bin/make qmake_all
QMAKE_PATH=$HOME/Qt/5.15.2/gcc_64/bin
mkdir -p build/xpcf
mkdir build/samplecomponent
mkdir build/testxpcf
pushd build/xpcf
${QMAKE_PATH}/qmake ../../../xpcf.pro ${QMAKE_OPTS}
make
make install
popd
pushd build/samplecomponent
${QMAKE_PATH}/qmake ../../../samples/sample_component.pro ${QMAKE_OPTS}
make
make install
popd
pushd build/testxpcf
${QMAKE_PATH}/qmake ../../../test/testxpcf.pro ${QMAKE_OPTS}
make
make install
./testxpcf
popd

