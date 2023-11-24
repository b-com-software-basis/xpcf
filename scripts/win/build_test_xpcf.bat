@echo off
cls
setlocal

REM default parameter value
set QTVERSION=5.14.2
set XPCFROOT=../..
set QMAKEPATH=C:\Qt\%QTVERSION%\msvc2017_64\bin
set JOMPATH=c:\Qt\Tools\QtCreator\bin\jom

REM check whether user had supplied -h or --help . If yes display usage 
for %%A in ("--help" "-h") do if "%1"==%%A (call:display_usage %1 & exit /b 0)

REM default win walues
if NOT [%1]==[] set XPCFROOT=%1
if NOT [%2]==[] set QTVERSION=%2
if NOT [%3]==[] set QMAKEPATH=%3
if NOT [%4]==[] set JOMPATH=%4

if not exist %XPCFROOT% (echo "XPCF project root path '%XPCFROOT%' doesn't exist" & exit /b 2)
echo "XPCF project root path used is : %XPCFROOT%"
if not exist %QMAKEPATH% (echo "qmake.exe path 'QMAKEPATH' doesn't exist" & exit /b 2)
echo "qmake path used is : %QMAKEPATH%"
if not exist %JOMPATH% (echo "jom.exe path 'JOMPATH' doesn't exist" & exit /b 2)
echo "jom path used is : %JOMPATH%"

if not exist %XPCFROOT%/scripts/win/build-xpcf (
	pause
	echo "%XPCFROOT%/scripts/win/build-xpcf path doesn't exist"
	echo "Running build_xpcf.bat"
	call %XPCFROOT%/scripts/win/build_xpcf.bat %XPCFROOT% %QTVERSION%
	pause
)
call %XPCFROOT%/scripts/win/build_remaken_project.bat testxpcf shared %XPCFROOT%/test %QTVERSION% %QMAKEPATH% %JOMPATH%
call %XPCFROOT%/scripts/win/build_remaken_project.bat testxpcf-static-deps static %XPCFROOT%/test %QTVERSION% %QMAKEPATH% %JOMPATH%

endlocal
goto:eof

::--------------------------------------------------------
::-- Function display_usage starts below here
::--------------------------------------------------------

:display_usage

echo This script builds %PROJECT% in shared mode.
echo It can receive two optional argument. 
echo.
echo Usage: param [path to xpcf project root - default='%XPCFROOT%'] [Qt kit version to use - default='%QTVERSION%'] [path to qmake.exe - default='%QMAKEPATH%'] [path to jom.exe='%JOMPATH%']
exit /b 0

:end
