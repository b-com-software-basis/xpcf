@echo off
cls
setlocal

set PKGVERSION=2.6.2

REM default parameter value
set QTVERSION=5.14.2
set XPCFROOT=../..

REM check whether user had supplied -h or --help . If yes display usage 
for %%A in ("--help" "-h") do if "%1"==%%A (call:display_usage %1 & exit /b 0)

REM default win walues
if NOT [%1]==[] set QMAKEPATH=%1
if NOT [%2]==[] set XPCFROOT=%2
if NOT [%3]==[] set QTVERSION=%3

call %XPCFROOT%/scripts/win/build_remaken_project_package.bat xpcf shared %QMAKEPATH% %XPCFROOT% %QTVERSION%
call %XPCFROOT%/scripts/win/build_remaken_project_package.bat xpcf static %QMAKEPATH% %XPCFROOT% %QTVERSION%
call %XPCFROOT%/scripts/win/build_remaken_project_package.bat xpcfSampleComponent shared %QMAKEPATH% %XPCFROOT%/samples/sample_component %QTVERSION%

call %XPCFROOT%/scripts/win/make_remaken_package.bat XPCF %PKGVERSION%
call %XPCFROOT%/scripts/win/make_remaken_package.bat xpcfSampleComponent %PKGVERSION%

endlocal
goto:eof


::--------------------------------------------------------
::-- Function display_usage starts below here
::--------------------------------------------------------

:display_usage

echo This script builds XPCF and xpcfSamplecomponent in shared and static mode with a special deployment option for packaging.
echo It also compress the various xpcf packages.
echo It can receive one optional argument. 
echo.
echo Usage: param [path to qmake executable] [path to xpcf] [Qt kit version to use - default='%QTVERSION%']
exit /b 0

:end
