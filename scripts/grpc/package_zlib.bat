@echo off
cls
setlocal

set PROJECT=zlib
set PKGVERSION=1.2.11

REM check whether user had supplied -h or --help . If yes display usage 
for %%A in ("--help" "-h") do if "%1"==%%A (call:display_usage %1 & exit /b 0)

call make_remaken_package.bat %PROJECT% %PKGVERSION%

endlocal
goto:eof


::--------------------------------------------------------
::-- Function display_usage starts below here
::--------------------------------------------------------

:display_usage

echo This script packages %PROJECT%.
echo.
exit /b 0

:end
