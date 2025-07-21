#!/bin/bash

XPCFROOT=../..
version = 2.10.0

pushd ${XPCFROOT}
scripts/unixes/update_version -f -v ${version}
popd

./package_xpcf.sh

# check /home/sleduc/_dev/swe/xpcf/tools/generators/grpc/GRPCProtoGenerator.cpp for grpc and protobuf version!
# => if grpc version changed => set a new xpcf_grpc_gen_bin version in https://github.com/b-com-software-basis/thirdparties-binaries/releases
#    with path to grpc release and protobuf (search conan local path)
# zip -r -y xpcf_grpc_gen_bin.zip xpcf_grpc_gen_bin
# upload zip on github

# upload manually on https://github.com/b-com-software-basis/xpcf/releases
# - release/tag xpcf/${version}/linux
#    - ~/.remaken/packages/linux-gcc/static/release/xpcf_${version}_x86_64_static_release.zip
#    - ~/.remaken/packages/linux-gcc/static/debug/xpcf_${version}_x86_64_static_debug.zip
#    - ~/.remaken/packages/linux-gcc/shared/release/xpcf_${version}_x86_64_shared_release.zip
#    - ~/.remaken/packages/linux-gcc/shared/debug/xpcf_${version}_x86_64_shared_debug.zip
# - release/tag xpcf_static_deps/${version}/linux
#    - ~/.remaken/packages/linux-gcc/shared/release/xpcf_static_deps_${version}_x86_64_shared_release.zip
#    - ~/.remaken/packages/linux-gcc/shared/debug/xpcf_static_deps_${version}_x86_64_shared_debug.zip
# - release/tag xpcf_grpc_gen/${version}/linux
#    - ~/.remaken/packages/linux-gcc/static/releasexpcf_grpc_gen_2.10.0_x86_64_static_release.zip
#    - ~/.remaken/packages/linux-gcc/static/debug/xpcf_grpc_gen_2.10.0_x86_64_static_debug.zip
