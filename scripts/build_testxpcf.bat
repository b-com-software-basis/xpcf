@echo off
setlocal

REM MSVC2017 & Qmake env
set VS2017_AMD64="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
set JOM_PATH=c:\Qt\Tools\QtCreator\bin
set QMAKE_PATH=C:\Qt\5.14.2\msvc2017_64\bin

echo "exec vcvars"
call %VS2017_AMD64%
echo "end exec vcvars"

if exist build\samplecomponent rmdir /S /Q build\samplecomponent
if not exist build\samplecomponent\debug mkdir build\samplecomponent\debug
if not exist build\samplecomponent\release mkdir build\samplecomponent\release

if exist build\testxpcf rmdir /S /Q build\testxpcf
if not exist build\testxpcf\debug mkdir build\testxpcf\debug
if not exist build\testxpcf\release mkdir build\testxpcf\release

echo "===========> building XPCF sample component <==========="
cd %~dp0\build\samplecomponent\debug
%QMAKE_PATH%\qmake.exe ../../../../samples/sample_component/xpcfSampleComponent.pro -spec win32-msvc CONFIG+=debug
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install
cd %~dp0\build\samplecomponent\release
%QMAKE_PATH%\qmake.exe ../../../../samples/sample_component/xpcfSampleComponent.pro -spec win32-msvc
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install

echo "===========> building XPCF testxpcf <==========="
cd %~dp0\build\testxpcf\debug
%QMAKE_PATH%\qmake.exe ../../../../test/testxpcf.pro -spec win32-msvc CONFIG+=debug
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install
cd %~dp0\build\testxpcf\release
%QMAKE_PATH%\qmake.exe ../../../../test/testxpcf.pro -spec win32-msvc
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install

:END
cd "%~dp0"

endlocal
goto:eof






