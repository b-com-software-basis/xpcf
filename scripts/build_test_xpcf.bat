@echo off
cls
setlocal

REM default parameter value
set QTVERSION=5.14.2

REM check whether user had supplied -h or --help . If yes display usage 
for %%A in ("--help" "-h") do if "%1"==%%A (call:display_usage %1 & exit /b 0)

REM replace default QTVERSION
if NOT [%1]==[] set QTVERSION=%1

call build_xpcf_shared.bat %QTVERSION%
call build_testxpcf.bat %QTVERSION%
REM call build_xpcf_static.bat %QTVERSION%

cd "%~dp0"

endlocal
goto:eof


::--------------------------------------------------------
::-- Function display_usage starts below here
::--------------------------------------------------------

:display_usage

echo This script builds %PROJECT% in shared mode.
echo It can receive one optional argument. 
echo.
echo Usage: param [Qt kit version to use - default='%QTVERSION%']
exit /b 0

:end






