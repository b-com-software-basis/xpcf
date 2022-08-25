@setlocal enableextensions enabledelayedexpansion
@echo off

REM #####################
REM Parameters processing
REM #####################
for %%A in (%*) do (
    if /I "%%A"=="--help" (call:display_usage %1 & exit /b 0)
    if /I "%%A"=="-h" (call:display_usage %1 & exit /b 0)
)

set year=""
set output=""

for %%A in (%*) do (
    if "!yearFound!"=="true" (
        set year=%%A
        goto yearParamProcessed
    )
    if "%%A"=="--year" set yearFound=true
    if "%%A"=="-y" set yearFound=true
)
:yearParamProcessed

for %%A in (%*) do (
    if "!outputFound!"=="true" (
        set output=%%A
        goto outputParamProcessed
    )
    if "%%A"=="--output" set outputFound=true
    if "%%A"=="-o" set outputFound=true
)
:outputParamProcessed

REM #####################################################
REM Search of the batch initializing compiler environment
REM #####################################################
if not exist "c:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" (
    echo "vswhere.exe not installed, unable to find compiler path"
    goto end
)

set path=""
for /F "delims=" %%i in ('"c:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -requires Microsoft.Component.MSBuild -property installationPath') do (
    if %year%=="" (
        set path=%%i
        goto displayPath
    )
    set temp=%%i
    set temp=!temp:%year%=!
    if not !temp!==%%i (
        set path=%%i
        goto displayPath
    )
)

:displayPath
if !path!=="" (
    echo "Error: no path found for %year%"
    goto end
)
if not %output%=="" (
    echo set Visual Studio environment: !path!\VC\Auxiliary\Build\vcvars64.bat
    echo !path!\VC\Auxiliary\Build\vcvars64.bat > %output%
) else (
    echo !path!\VC\Auxiliary\Build\vcvars64.bat
)

goto end

:display_usage
echo This script searches for the path of the batch to execute to set up environment for Visual Studio compiler and writes it in a file
echo It takes the year of Visual Studio (2017, 2019, ...) and a file path to write the result.
echo
echo init_compiler_env_script [option <value>]
echo option:
echo    -   --help or -h        display usage
echo    -   --year or -y        define year of Visual Studio to search
echo    -   --output or -o      define file path to write path of the batch to execute to set up the environment of Visual Studio compiler         
exit /b 0

:end