@echo off
cls
setlocal
set rootDir=%~dp0

REM #################### PARAM #####################
rem %1 = config => debug or release
REM ################################################

if [%1]==[] goto usage
goto :exec
:usage
@echo Usage: %0 ^<config debug/release^>
goto :eof

:exec
REM ####################
REM REMAKEN_PKG_ROOT
REM ####################
set REMAKEN_PKG_ROOT_WIN=%USERPROFILE%\.remaken\packages
echo "REMAKEN_PKG_ROOT_WIN:%REMAKEN_PKG_ROOT_WIN%"

REM ####################
REM Product info
REM ####################
set product=zlib
set version=1.2.11
set compiler=win-cl-14.1
set config=%1

REM ####################
REM Git
REM ####################
git clone https://github.com/madler/zlib.git
cd zlib
git checkout v%version%
git submodule init
git submodule update

REM ####################
REM build
REM ####################
set VS2017_AMD64="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
set PATH=%PATH%
call %VS2017_AMD64%

cmake.exe -G"Visual Studio 15 2017" -A x64 -B "_BUILD64_%config%" -DCMAKE_INSTALL_PREFIX=%rootDir%%product%_install_%config%

devenv _BUILD64_%config%\zlib.sln /build %config% /Project ALL_BUILD
IF ERRORLEVEL 1 GOTO error

devenv _BUILD64_%config%\zlib.sln /build %config% /Project INSTALL
IF ERRORLEVEL 1 GOTO error

REM ####################
REM packaging
REM ####################
cd %rootDir%
set dir=%REMAKEN_PKG_ROOT_WIN%\%compiler%\static\%config%\%product%\%version%
echo "product install dir:%dir%

mkdir %dir%\interfaces
mkdir %dir%\lib\x86_64\static\%config%

REM TODO make a bcom-zlib.pc
REM copy /y bcom-grpc.pc %dir% 
xcopy /y /s %rootDir%%product%_install_%config%\include %dir%\interfaces
xcopy /y /s %rootDir%%product%_install_%config%\lib %dir%\lib\x86_64\static\%config%

:ok
REM exit 0

:error
REM exit 1