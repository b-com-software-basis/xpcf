#!/bin/bash
QMAKE_PATH=$HOME/Qt/5.15.2/gcc_64/bin
mkdir -p build/xpcf_shared
mkdir -p build/xpcf_static
mkdir build/samplecomponent
mkdir build/testxpcf
mkdir build/xpcf_grpc_gen
echo "===========> building XPCF shared <==========="
pushd build/xpcf_shared
`${QMAKE_PATH}/qmake ../../../xpcf.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
rm *
`${QMAKE_PATH}/qmake ../../../xpcf.pro -spec linux-g++ CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd
echo "===========> building XPCF static <==========="
pushd build/xpcf_static
`${QMAKE_PATH}/qmake ../../../xpcf.pro -spec linux-g++  CONFIG+=debug CONFIG+=qml_debug CONFIG+=staticlib && /usr/bin/make qmake_all`
make
make install
rm *
`${QMAKE_PATH}/qmake ../../../xpcf.pro -spec linux-g++ CONFIG+=qml_debug CONFIG+=staticlib && /usr/bin/make qmake_all`
make
make install
popd
echo "===========> building XPCF sample component <==========="
pushd build/samplecomponent
`${QMAKE_PATH}/qmake ../../../samples/sample_component/xpcfSampleComponent.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
rm *
`${QMAKE_PATH}/qmake ../../../samples/sample_component/xpcfSampleComponent.pro -spec linux-g++ CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd
echo "===========> building testxpcf <==========="
pushd build/testxpcf
`${QMAKE_PATH}/qmake ../../../test/testxpcf.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
rm *
`${QMAKE_PATH}/qmake ../../../test/testxpcf.pro -spec linux-g++ CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd
echo "===========> building cppast <==========="
pushd ../libs
./build_cppast.sh
popd
echo "===========> building xpcf_grpc_gen <==========="
pushd build/xpcf_grpc_gen
`${QMAKE_PATH}/qmake ../../../tools/generators/grpc/xpcf_grpc_gen.pro -spec linux-g++ CONFIG+=debug CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
rm *
`${QMAKE_PATH}/qmake ../../../tools/generators/grpc/xpcf_grpc_gen.pro -spec linux-g++ CONFIG+=qml_debug && /usr/bin/make qmake_all`
make
make install
popd

