@echo off
setlocal

set PROJECT=xpcf

REM default parameter value
set QTVERSION=5.14.2

REM check whether user had supplied -h or --help . If yes display usage 
for %%A in ("--help" "-h") do if "%1"==%%A (call:display_usage %1 & exit /b 0)

REM replace default QTVERSION
if NOT [%1]==[] set QTVERSION=%1

set JOM_PATH=c:\Qt\Tools\QtCreator\bin
set QMAKE_PATH=C:\Qt\%QTVERSION%\msvc2017_64\bin

if not exist %QMAKE_PATH% (echo "Qt path '%QMAKE_PATH%' doesn't exist : check your Qt installation and kits" & exit /b 2)

call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"

if exist build-%PROJECT%\samplecomponent rmdir /S /Q build-%PROJECT%\samplecomponent
if not exist build-%PROJECT%\samplecomponent\debug mkdir build-%PROJECT%\samplecomponent\debug
if not exist build-%PROJECT%\samplecomponent\release mkdir build-%PROJECT%\samplecomponent\release

if exist build-%PROJECT%\testxpcf rmdir /S /Q build-%PROJECT%\testxpcf
if not exist build-%PROJECT%\testxpcf\debug mkdir build-%PROJECT%\testxpcf\debug
if not exist build-%PROJECT%\testxpcf\release mkdir build-%PROJECT%\testxpcf\release

echo "===========> building XPCF sample component <==========="
cd %~dp0\build-%PROJECT%\samplecomponent\debug
%QMAKE_PATH%\qmake.exe ../../../../samples/sample_component/xpcfSampleComponent.pro -spec win32-msvc CONFIG+=debug
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install
cd %~dp0\build-%PROJECT%\samplecomponent\release
%QMAKE_PATH%\qmake.exe ../../../../samples/sample_component/xpcfSampleComponent.pro -spec win32-msvc
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install

echo "===========> building XPCF testxpcf <==========="
cd %~dp0\build-%PROJECT%\testxpcf\debug
%QMAKE_PATH%\qmake.exe ../../../../test/testxpcf.pro -spec win32-msvc CONFIG+=debug
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install
cd %~dp0\build-%PROJECT%\testxpcf\release
%QMAKE_PATH%\qmake.exe ../../../../test/testxpcf.pro -spec win32-msvc
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install

:END
cd "%~dp0"

endlocal
goto:eof

::--------------------------------------------------------
::-- Function display_usage starts below here
::--------------------------------------------------------

:display_usage

echo This script builds testxpcf.
echo It can receive one optional argument. 
echo.
echo Usage: param [Qt kit version to use - default='%QTVERSION%']
exit /b 0

:end