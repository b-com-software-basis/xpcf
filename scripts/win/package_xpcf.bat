@echo off
cls
setlocal

set PROJECT=xpcf
set PKGVERSION=2.5.1

REM default parameter value
set QTVERSION=5.14.2
set XPCFROOT=../..

REM check whether user had supplied -h or --help . If yes display usage 
for %%A in ("--help" "-h") do if "%1"==%%A (call:display_usage %1 & exit /b 0)

REM default win walues
if NOT [%1]==[] set XPCFROOT=%1
if NOT [%2]==[] set QTVERSION=%2

set JOM_PATH=c:\Qt\Tools\QtCreator\bin\jom
set QMAKE_PATH=C:\Qt\%QTVERSION%\msvc2017_64\bin

if not exist %QMAKE_PATH% (echo "Qt path '%QMAKE_PATH%' doesn't exist : check your Qt installation and kits" & exit /b 2)
if not exist %XPCFROOT% (echo "XPCF project root path '%XPCFROOT%' doesn't exist" & exit /b 2)
echo "XPCF project root path used is : %XPCFROOT%"

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"

if exist build-%PROJECT%-package\shared rmdir /S /Q build-%PROJECT%-package\shared
if not exist build-%PROJECT%-package\shared\debug mkdir build-%PROJECT%-package\shared\debug
if not exist build-%PROJECT%-package\shared\release mkdir build-%PROJECT%-package\shared\release

REM echo "===========> building XPCF shared <==========="
pushd build-%PROJECT%-package\shared\debug
%QMAKE_PATH%\qmake.exe ../../../%XPCFROOT%/%PROJECT%.pro -spec win32-msvc CONFIG+=debug CONFIG+=package_remaken
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install
popd
pushd build-%PROJECT%-package\shared\release
%QMAKE_PATH%\qmake.exe ../../../%XPCFROOT%/%PROJECT%.pro -spec win32-msvc CONFIG+=package_remaken
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install
popd

if exist build-%PROJECT%-package\static rmdir /S /Q build-%PROJECT%-package\static
if not exist build-%PROJECT%-package\static\debug mkdir build-%PROJECT%-package\static\debug
if not exist build-%PROJECT%-package\static\release mkdir build-%PROJECT%-package\static\release

REM echo "===========> building XPCF static <==========="
pushd build-%PROJECT%-package\static\debug
%QMAKE_PATH%\qmake.exe ../../../%XPCFROOT%/%PROJECT%.pro -spec win32-msvc CONFIG+=staticlib CONFIG+=debug CONFIG+=package_remaken
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install
popd
pushd build-%PROJECT%-package\static\release
%QMAKE_PATH%\qmake.exe ../../../%XPCFROOT%/%PROJECT%.pro -spec win32-msvc CONFIG+=staticlib CONFIG+=package_remaken
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install
popd

call %XPCFROOT%/scripts/win/make_remaken_package.bat %PROJECT% %PKGVERSION%

endlocal
goto:eof


::--------------------------------------------------------
::-- Function display_usage starts below here
::--------------------------------------------------------

:display_usage

echo This script builds %PROJECT% in shared with a special deployment option for packaging.
echo It also compress the various xpcf packages.
echo It can receive one optional argument. 
echo.
echo Usage: param [Qt kit version to use - default='%QTVERSION%']
exit /b 0

:end
