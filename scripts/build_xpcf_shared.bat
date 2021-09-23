@echo off
setlocal

REM MSVC2017 & Qmake env
set VS2017_AMD64="C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
set JOM_PATH=c:\Qt\Tools\QtCreator\bin
set QMAKE_PATH=C:\Qt\5.14.2\msvc2017_64\bin

echo "exec vcvars"
call %VS2017_AMD64%
echo "end exec vcvars"

if exist build\xpcf_shared rmdir /S /Q build\xpcf_shared
if not exist build\xpcf_shared\debug mkdir build\xpcf_shared\debug
if not exist build\xpcf_shared\release mkdir build\xpcf_shared\release

echo "===========> building XPCF shared <==========="
cd %~dp0\build\xpcf_shared\debug
%QMAKE_PATH%\qmake.exe ../../../../xpcf.pro -spec win32-msvc CONFIG+=debug
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install

cd %~dp0\build\xpcf_shared\release
%QMAKE_PATH%\qmake.exe ../../../../xpcf.pro -spec win32-msvc
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install

:END
cd "%~dp0"

endlocal
goto:eof






