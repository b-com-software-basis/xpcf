@echo off

set BUILDFOLDER=build-cppast
set BUILDTYPE=release

@REM Check parameters
set argc=0
for %%x in (%*) do set /A argc+=1
if %argc% gtr 0 (
    goto display_usage
)

@REM #####################################################
@REM Configure LLVM
@REM #####################################################

@REM Install llvm 14.0.0 because we already compiled llvm-config for this version.
call choco install llvm -y --version 14.0.0

@REM Get llvm-config.exe
call choco install curl -y
call curl https://github.com/b-com-software-basis/thirdparties-binaries/releases/download/llvm-config/13.0.0/win/llvm-config.exe -O -L -J

call move llvm-config.exe "c:\Program Files\LLVM\bin\"
if not exist "c:\Program Files\LLVM\bin\llvm-config.exe" (
    goto error_llvm_config
)

@REM #####################################################
@REM Build cppast
@REM #####################################################
@REM Patche cppast


@REM Compile cppAst
call choco install cmake

set BUILDPATH=%BUILDFOLDER%-%BUILDTYPE%\%TARGETPLATFORM%
mkdir %BUILDPATH%
cd %BUILDPATH%

call cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDANT_CODE=ON -G "Unix Makefiles" ..\cppast
call make
call make install
goto end

:error_llvm_config
echo Error - Enable to find llvm path to copy llvm-config.exe.

:display_usage
echo Batch to compile cppast project. 
echo /!\ Must be executed with administrator rights to install LLVM, curl and cmake. /!\
goto bad_end

:end
echo cppast build finished

:bad_end