@echo off
REM cls
setlocal
set rootDir=%~dp0

REM default parameter value
set OPENSSL_DEFAULT_MODE=module

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

REM replace default OPENSSL_DEFAULT_MODE
if NOT [%2]==[] set OPENSSL_DEFAULT_MODE=%2

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
cd ..

REM ####################
REM Conan Openssl deps
REM ####################
setlocal EnableDelayedExpansion
if /I %OPENSSL_DEFAULT_MODE%==package (
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

cmake.exe -G"Visual Studio 15 2017" -A x64 -B "grpc/_BUILD64_%config%" -DABSL_ENABLE_INSTALL=1 ^
-DgRPC_ZLIB_PROVIDER=package ^
-DZLIB_INCLUDE_DIR=%zlibPath%/interfaces ^
-DZLIB_LIBRARY_DEBUG=%zlibPath%/lib/x86_64/static/debug/zlibstaticd.lib ^
-DZLIB_LIBRARY_RELEASE=%zlibPath%/lib/x86_64/static/release/zlibstatic.lib ^
-DCMAKE_INSTALL_PREFIX=%rootDir%%product%_install_%config% ^
-DgRPC_INSTALL=ON ^
-DgRPC_SSL_PROVIDER=%OPENSSL_DEFAULT_MODE%

REM gRPC_INSTALL add because value changed !? (from used of root cmakelist?)

devenv grpc\_BUILD64_%config%\grpc_wrapper.sln /build %config% /Project ALL_BUILD
IF ERRORLEVEL 1 GOTO error

devenv grpc\_BUILD64_%config%\grpc_wrapper.sln /build %config% /Project INSTALL
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
copy /y "%rootDir%bcom-%suffix%grpc++-win.pc" "%dir%\bcom-%suffix%grpc++.pc"

xcopy /y /s %rootDir%%product%_install_%config%\include %dir%\interfaces
xcopy /y /s %rootDir%%product%_install_%config%\lib %dir%\lib\x86_64\static\%config%
xcopy /y /s %rootDir%%product%_install_%config%\bin %dir%\bin\x86_64\static\%config%

REM zlib
if /I %config%==debug (set suffix=d) else (set suffix=)
call set zlibPath=%%zlibPath:/=\%%
copy /y %zlibPath%\lib\x86_64\static\%config%\zlibstatic%suffix%.lib %dir%\lib\x86_64\static\%config%\zlib.lib

REM ssl ('package' mode only)
setlocal EnableDelayedExpansion
if /I %OPENSSL_DEFAULT_MODE%==package (
	for /f %%a in ('call getnexttext.bat %~dp0_build_openssl\conanbuildinfo.txt [libdirs_openssl]') do set "openssl_libdir=%%a"
	call set openssl_libdir_BACKSLASH=%%openssl_libdir:/=\%%

	copy /y !openssl_libdir_BACKSLASH!\libssl%suffix%.lib %dir%\lib\x86_64\static\%config%\ssl.lib
	copy /y !openssl_libdir_BACKSLASH!\libcrypto%suffix%.lib %dir%\lib\x86_64\static\%config%\crypto.lib
)
endlocal

endlocal
goto:eof


::--------------------------------------------------------
::-- Function display_usage starts below here
::--------------------------------------------------------

:display_usage

echo This script builds grpc in static mode.
echo It expects one mandatory argument and also can receive one optional argument. 
echo.
echo Usage: params [remaken package name] [OpenSSL version - default='module' (internal boringssl) or 'package' with a conan openssl 1.1.1k version]
exit /b 0

:end



@echo Usage: %0 ^<config debug/release^>
goto :eof


:ok
REM exit 0

:error
REM exit 1