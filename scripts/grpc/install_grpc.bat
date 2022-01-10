@echo off
cls
setlocal

REM default parameter value - build with boringssl
set OPENSSL_BUILD=boringssl

set PROJECT=grpc
set PKGVERSION=1.37.1
set REMAKEN_PLATFORM=win-cl-14.1

REM check whether user had supplied -h or --help . If yes display usage 
for %%A in ("--help" "-h") do if "%1"==%%A (call:display_usage %1 & exit /b 0)

REM replace default OPENSSL_BUILD
if NOT [%1]==[] (set OPENSSL_BUILD=%1)
if /I %OPENSSL_BUILD%==openssl (
	set PROJECT=grpc-%OPENSSL_BUILD%
)

REM ####################
REM REMAKEN_PKG_ROOT
REM ####################
set REMAKEN_PKG_ROOT_WIN=%USERPROFILE%\.remaken\packages
echo "REMAKEN_PKG_ROOT_WIN:%REMAKEN_PKG_ROOT_WIN%"

set input_dir=%REMAKEN_PKG_ROOT_WIN%\%REMAKEN_PLATFORM%\static
set output_dir=%REMAKEN_PKG_ROOT_WIN%\%REMAKEN_PLATFORM%

xcopy /y /s %input_dir%\debug\%PROJECT%\%PKGVERSION%\ %output_dir%\%PROJECT%\%PKGVERSION%\
xcopy /y /s %input_dir%\release\%PROJECT%\%PKGVERSION%\ %output_dir%\%PROJECT%\%PKGVERSION%\

endlocal
goto:eof

::--------------------------------------------------------
::-- Function display_usage starts below here
::--------------------------------------------------------

:display_usage

echo This script install %PROJECT%.
echo It can receive one optional argument. 
echo.
echo Usage: [build with OpenSSL - default='boringssl' (internal boringssl) or 'openssl' with a conan openssl 1.1.1k version]
exit /b 0

:end
