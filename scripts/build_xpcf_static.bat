@echo off
setlocal

REM MSVC2017 & Qmake env
set VS2017_AMD64="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
set JOM_PATH=c:\Qt\Tools\QtCreator\bin
set QMAKE_PATH=C:\Qt\5.14.2\msvc2017_64\bin

echo "exec vcvars"
call %VS2017_AMD64%
echo "end exec vcvars"

if exist build\xpcf_static rmdir /S /Q build\xpcf_static
if not exist build\xpcf_static\debug mkdir build\xpcf_static\debug
if not exist build\xpcf_static\release mkdir build\xpcf_static\release

echo "===========> building XPCF static <==========="
cd %~dp0\build\xpcf_static\debug
%QMAKE_PATH%\qmake.exe ../../../../xpcf.pro -spec win32-msvc CONFIG+=debug CONFIG+=staticlib DEFINES+=xpcf_API_DLLEXPORT
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install
cd %~dp0\build\xpcf_static\release
%QMAKE_PATH%\qmake.exe ../../../../xpcf.pro -spec win32-msvc CONFIG+=staticlib DEFINES+=xpcf_API_DLLEXPORT
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install

:END
cd "%~dp0"

endlocal
goto:eof






