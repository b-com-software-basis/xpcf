@echo off
REM cls
setlocal
set rootDir=%~dp0

REM default parameter value - build with boringssl
set OPENSSL_BUILD=boringssl

REM check whether user had supplied -h or --help . If yes display usage 
for %%A in ("--help" "-h") do if "%1"==%%A (call:display_usage %1 & exit /b 0)

REM #################### PARAM #####################
rem %1 = config => debug or release
REM ################################################

if [%1]==[] goto usage
goto :exec
:usage
call:display_usage
exit /b 1
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


REM replace default OPENSSL_BUILD
if NOT [%2]==[] (set OPENSSL_BUILD=%2)
set SSL_DEFAULT_MODE=module
if /I %OPENSSL_BUILD%==openssl (
	set SSL_DEFAULT_MODE=package
	set product=grpc-%OPENSSL_BUILD%
)

REM ####################
REM Git
REM ####################
git clone https://github.com/grpc/grpc.git
cd grpc
git checkout v%version%
git submodule init
git submodule update
cd ..

REM ####################
REM Conan Openssl deps
REM ####################
setlocal EnableDelayedExpansion
if /I %SSL_DEFAULT_MODE%==package (
	if /I %config%==debug (set conanbuildtype=Debug) else (set conanbuildtype=Release)
	conan install conanfile_openssl.txt -s arch=x86_64 -s compiler.cppstd=17 -s build_type=!conanbuildtype! --build=missing -if %~dp0_build_openssl
)
endlocal

REM ####################
REM build
REM ####################
set VS2017_AMD64="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
set PATH=%PATH%
call %VS2017_AMD64%

call set REMAKEN_PKG_ROOT_WIN_SLASH=%%REMAKEN_PKG_ROOT_WIN:\=/%%
set zlibPath=%REMAKEN_PKG_ROOT_WIN_SLASH%/%compiler%/zlib/1.2.11

cmake.exe -G"Visual Studio 15 2017" -A x64 -B "grpc/_BUILD64_%config%_%OPENSSL_BUILD%" -DABSL_ENABLE_INSTALL=1 ^
-DgRPC_ZLIB_PROVIDER=package ^
-DZLIB_INCLUDE_DIR=%zlibPath%/interfaces ^
-DZLIB_LIBRARY_DEBUG=%zlibPath%/lib/x86_64/static/debug/zlibstaticd.lib ^
-DZLIB_LIBRARY_RELEASE=%zlibPath%/lib/x86_64/static/release/zlibstatic.lib ^
-DCMAKE_INSTALL_PREFIX=%rootDir%%product%_install_%config% ^
-DgRPC_INSTALL=ON ^
-DgRPC_SSL_PROVIDER=%SSL_DEFAULT_MODE%

REM gRPC_INSTALL add because value changed !? (from used of root cmakelist?)

devenv grpc\_BUILD64_%config%_%OPENSSL_BUILD%\grpc_wrapper.sln /build %config% /Project ALL_BUILD
IF ERRORLEVEL 1 GOTO error

devenv grpc\_BUILD64_%config%_%OPENSSL_BUILD%\grpc_wrapper.sln /build %config% /Project INSTALL
IF ERRORLEVEL 1 GOTO error

REM ####################
REM packaging
REM ####################
cd %rootDir%
REM set dir=%REMAKEN_PKG_ROOT_WIN%\%compiler%\%product%\%version%
set dir=%REMAKEN_PKG_ROOT_WIN%\%compiler%\static\%config%\%product%\%version%
echo "product install dir:%dir%

mkdir %dir%\interfaces
mkdir %dir%\lib\x86_64\static\%config%
mkdir %dir%\bin\x86_64\static\%config%

if /I %config%==debug (set suffix=debug-) else (set suffix=)
if /I %SSL_DEFAULT_MODE%==package (set sslpackage=-openssl) else (set sslpackage=)
copy /y "%rootDir%bcom-%suffix%grpc++-win%sslpackage%.pc" "%dir%\bcom-%suffix%grpc++.pc"

xcopy /y /s %rootDir%%product%_install_%config%\include %dir%\interfaces
xcopy /y /s %rootDir%%product%_install_%config%\lib %dir%\lib\x86_64\static\%config%
xcopy /y /s %rootDir%%product%_install_%config%\bin %dir%\bin\x86_64\static\%config%

REM zlib
if /I %config%==debug (set suffix=d) else (set suffix=)
call set zlibPath=%%zlibPath:/=\%%
copy /y %zlibPath%\lib\x86_64\static\%config%\zlibstatic%suffix%.lib %dir%\lib\x86_64\static\%config%\zlib.lib

endlocal
goto:eof


::--------------------------------------------------------
::-- Function display_usage starts below here
::--------------------------------------------------------

:display_usage

echo This script builds grpc in static mode.
echo It expects one mandatory argument and also can receive one optional argument. 
echo.
echo Usage: params [config debug/release] [build with OpenSSL - default='boringssl' (internal boringssl) or 'openssl' with a conan openssl 1.1.1k version]
exit /b 0

:end

goto :eof

:ok
REM exit 0

:error
REM exit 1