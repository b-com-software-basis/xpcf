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
set product=grpc
set version=1.37.1
set compiler=win-cl-14.1
set config=%1

REM ####################
REM Git
REM ####################
git clone https://github.com/grpc/grpc.git
cd grpc
git checkout v%version%
git submodule init
git submodule update

REM ####################
REM build
REM ####################
set VS2017_AMD64="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
set PATH=%PATH%
call %VS2017_AMD64%

call set REMAKEN_PKG_ROOT_WIN_SLASH=%%REMAKEN_PKG_ROOT_WIN:\=/%%
set zlibPath=%REMAKEN_PKG_ROOT_WIN_SLASH%/%compiler%/zlib/1.2.11

cmake.exe -G"Visual Studio 15 2017" -A x64 -B "_BUILD64_%config%" -DABSL_ENABLE_INSTALL=1 -DgRPC_ZLIB_PROVIDER=package -DZLIB_INCLUDE_DIR=%zlibPath%/interfaces -DZLIB_LIBRARY_DEBUG=%zlibPath%/lib/x86_64/static/debug/zlibstaticd.lib -DZLIB_LIBRARY_RELEASE=%zlibPath%/lib/x86_64/static/release/zlibstatic.lib  -DCMAKE_INSTALL_PREFIX=%rootDir%%product%_install_%config%


devenv _BUILD64_%config%\grpc.sln /build %config% /Project ALL_BUILD
IF ERRORLEVEL 1 GOTO error

devenv _BUILD64_%config%\grpc.sln /build %config% /Project INSTALL
IF ERRORLEVEL 1 GOTO error

REM ####################
REM packaging
REM ####################
cd %rootDir%
set dir=%REMAKEN_PKG_ROOT_WIN%\%compiler%\%product%\%version%
echo "product install dir:%dir%

mkdir %dir%\interfaces
mkdir %dir%\lib\x86_64\static\%config%
mkdir %dir%\bin\x86_64\static\%config%

copy /y bcom-grpc++-win.pc %dir%\bcom-grpc++.pc
xcopy /y /s %rootDir%%product%_install_%config%\include %dir%\interfaces
xcopy /y /s %rootDir%%product%_install_%config%\lib %dir%\lib\x86_64\static\%config%
xcopy /y /s %rootDir%%product%_install_%config%\bin %dir%\bin\x86_64\static\%config%

if /I %config%==debug (set suffix=d) else (set suffix=)
call set zlibPath=%%zlibPath:/=\%%
copy /y %zlibPath%\lib\x86_64\static\%config%\zlibstatic%suffix%.lib %dir%\lib\x86_64\static\%config%\zlib.lib

:ok
REM exit 0

:error
REM exit 1