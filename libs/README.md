Cppast project build
=========================================
This README describes how to build cppast with clang 13.0.0. 

# llvm-config.exe generation
Usually llvm-config.exe is already built and stored on [github](https://github.com/b-com-software-basis/thirdparties-binaries).
This chapter describes how to build a specific version.

  - Get llvm-project source code

1/ Clone the git repository llvm-project 
    git clone https://github.com/llvm/llvm-project.git
2/ Checkout to your specific version. Tags are commonly like llvmorg-<version> (tag llvmorg-13.0.0 for clang version 13.0.0).
    git checkout llvmorg-13.0.0

  - generate the visual studio solution to compile the binary

1/ cd llvm-project
2/ mkdir build
3/ cd build

vs2017 4/ cmake -DLLVM_ENABLE_PROJECTS="clang" -DLLVM_TARGETS_TO_BUILD="X86" -G "Visual Studio 15 2017" -Thost=x64 ..\llvm
vs2022 4/ cmake -DLLVM_ENABLE_PROJECTS="clang" -DLLVM_TARGETS_TO_BUILD="X86" -G "Visual Studio 17 2022" -Thost=x64 ..\llvm

  - generate llvm-config.exe

1/ open llvm.sln with the good visual studio application
2/ change configuration to Release
3/ generate the project Tools\llvm-config

You will find llvm-config.exe in the folder **build/Release/bin**.

  - store llvm-config.exe

To be used by the script below, you need to store your specific version on github [github](https://github.com/b-com-software-basis/thirdparties-binaries).

# cppast projet build
You will find two scripts to build cppast project:
  * Linux: use **<xpcf_repo>/libs/build_cppast.sh**
  * Windows: use **<xpcf_repo>/libs/build_cppast.bat** (**/!\** Administrator rights are needed to install packages **/!\**)

This two scripts need to be executed in the folder **<xpcf_repo>/libs**.