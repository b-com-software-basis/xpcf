## Scripts informations

- version.sh : used to set version for scripts (called by other scripts)
- release_version.sh : for package xpcf binaries before upload
- update_version.sh : update template files for define xpcf version
- build_xpcf_grpc_test_sample.sh : file generated from template, in order to build grpc samples. don't forget to re-generate compilecommande.json for SampleComponent lib before : 
    QtCreator on xpcfSampleComponent project : build->run Generator -> Compilation Database
	file compile_commands.json generated in subfolder \build\Desktop_Qt_5_15_2_GCC_64bit-Debug or Release
	Copy to root xpcfSampleComponent project dir and replace file
- build_all.sh : build xpcf, test, grpc tools, and samples
- remaken_install_for_test.sh : make install of all dependencies after a build_all (for instance). based on generated packagedependencies.txt files (TODO change to projects files)
- remaken_bundle_for_tst.sh : bundle all test binairies (copy shared dependencies) : xpcf_grpc_client, xpcf_grpc_server, xpcfGrpcRemotingxpcfSampleComponent, testXpcf
- remaken_remove_version.sh : remove a specific version of all binaires in  ~/.remaken/packages

## Tests

- xml files ares installed in ~/.xpcf : xml contains path to xpcfsamplecomponent
- testxpcf binary don't need parameter

## grpc tests : 
./xpcf_grpc_server -f ~/.xpcf/grpc/xpcfGrpcSampleComponentServer.xml 
./xpcf_grpc_client -f ~/.xpcf/grpc/xpcfGrpcSampleComponentClient.xml 
