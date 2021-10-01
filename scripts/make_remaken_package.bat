@echo off
setlocal

REM check whether user had supplied -h or --help . If yes display usage 
for %%A in ("--help" "-h") do if "%1"==%%A (call:display_usage & exit /b 0)

if "%2" == "" goto args_count_ko
if "%3" == "" goto args_count_ok
:args_count_ko
call:display_usage
exit /b 1
:args_count_ok

echo "===========> package XPCF shared <==========="
set PKGNAME=%1
set PKGVERSION=%2
set REMAKEN_PLATFORM=win-cl-14.1
set BUILDMODES=debug release
set LINKMODES=shared static

for %%b in (%BUILDMODES%) do (
	REM https://stackoverflow.com/questions/8648178/getting-substring-of-a-token-in-for-loop
	setlocal EnableDelayedExpansion
	for %%l in (%LINKMODES%) do (
		SET BUILDFOLDER=%USERPROFILE%\.remaken\packages\%REMAKEN_PLATFORM%\%%l\%%b
		if exist !BUILDFOLDER!\%PKGNAME% (
			echo Packaging %PKGNAME%-%PKGVERSION% from folder !BUILDFOLDER!\%PKGNAME%
			7z a -tzip %PKGNAME%_%PKGVERSION%_x86_64_%%l_%%b.zip !BUILDFOLDER!\%PKGNAME% >NUL
		)
	)
)

cd "%~dp0"

endlocal
goto:eof


::--------------------------------------------------------
::-- Function display_usage starts below here
::--------------------------------------------------------

:display_usage

echo This compress a remaken built package in order to deploy the compressed result on a remote.
echo It expects two arguments. 
echo.
echo Usage: params [remaken package name] [remaken package version]
exit /b 0

:end


















display_usage() { 
	echo "This script compress a remaken built package in order to deploy the compressed result on a remote."
    echo "It expects two arguments." 
	echo -e "\nUsage: \$0 [remaken package name] [remaken package version] \n" 
} 

if [  $# -lt 2 ] 
then 
    display_usage
    exit 1
fi 

# check whether user had supplied -h or --help . If yes display usage 
if [[ ( $1 == "--help") ||  $1 == "-h" ]] 
then 
    display_usage
    exit 0
fi 

PKGNAME=$1
PKGVERSION=$2

REMAKEN_PLATFORM=linux-gcc
if [[ "$OSTYPE" == "darwin"* ]]; then
# overload for mac values
	REMAKEN_PLATFORM=mac-clang
fi

BUILDMODES="debug release"
LINKMODES="static shared"
for linkMode in ${LINKMODES}
do
for buildMode in ${BUILDMODES}
do
BUILDFOLDER=$HOME/.remaken/packages/${REMAKEN_PLATFORM}/${linkMode}/${buildMode}
if [ -d ${BUILDFOLDER}/${PKGNAME} ]
then
echo "Packaging ${PKGNAME}-${PKGVERSION} from folder ${BUILDFOLDER}/${PKGNAME}"
pushd ${BUILDFOLDER}
zip -r ${PKGNAME}_${PKGVERSION}_x86_64_${linkMode}_${buildMode}.zip ${PKGNAME}
popd
fi
done
done
