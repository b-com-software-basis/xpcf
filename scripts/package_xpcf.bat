@echo off
cls
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

REM echo "===========> building XPCF shared <==========="
cd %~dp0\build\xpcf_shared\debug
%QMAKE_PATH%\qmake.exe ../../../../xpcf.pro -spec win32-msvc CONFIG+=debug CONFIG+=package_remaken
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install

cd %~dp0\build\xpcf_shared\release
%QMAKE_PATH%\qmake.exe ../../../../xpcf.pro -spec win32-msvc CONFIG+=package_remaken
%JOM_PATH%\jom.exe
%JOM_PATH%\jom.exe install

cd "%~dp0"

echo "===========> package XPCF shared <==========="
set PKGNAME=xpcf
set PKGVERSION=2.5.0
set BUILDMODES=debug release
set LINKMODES=shared
REM set LINKMODES=shared static

for %%b in (%BUILDMODES%) do (
	REM https://stackoverflow.com/questions/8648178/getting-substring-of-a-token-in-for-loop
	setlocal EnableDelayedExpansion
	for %%l in (%LINKMODES%) do (
		SET BUILDFOLDER=%USERPROFILE%\.remaken\packages\win-cl-14.1\%%l\%%b
		if exist !BUILDFOLDER! (
			7z a -tzip %PKGNAME%_%PKGVERSION%_x86_64_%%l_%%b.zip !BUILDFOLDER!\%PKGNAME%
		)
	)
)

:END
cd "%~dp0"

endlocal
goto:eof






