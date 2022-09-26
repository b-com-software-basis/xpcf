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

echo "===========> package <==========="
set PKGNAME=%1
set PKGVERSION=%2
set REMAKEN_PLATFORM=win-cl-14.1
set BUILDMODES=debug release
set LINKMODES=shared static

set PKGROOT=%PKGNAME%\%PKGVERSION%
mkdir %PKGROOT%

for %%b in (%BUILDMODES%) do (
	REM https://stackoverflow.com/questions/8648178/getting-substring-of-a-token-in-for-loop
	setlocal EnableDelayedExpansion
	for %%l in (%LINKMODES%) do (
		SET BUILDFOLDER=%USERPROFILE%\.remaken\packages\%REMAKEN_PLATFORM%\%PKGNAME%\%PKGVERSION%
		if exist !BUILDFOLDER! (
			if exist !BUILDFOLDER!\interfaces (
				if exist !BUILDFOLDER!\lib\x86_64\%%l\%%b (
					@REM Copy interface folder
					mkdir %PKGROOT%\interfaces
					xcopy /y /s /E /q !BUILDFOLDER!\interfaces %PKGROOT%\interfaces
					@REM Copy lib folder
					mkdir %PKGROOT%\lib\x86_64\%%l\%%b
					xcopy /y /s /E /q !BUILDFOLDER!\lib\x86_64\%%l\%%b %PKGROOT%\lib\x86_64\%%l\%%b
					if exist !BUILDFOLDER!\wizards (
						@REM Copy wizards folder
						mkdir %PKGROOT%\wizards
						xcopy /y /s /E /q !BUILDFOLDER!\wizards %PKGROOT%\wizards
					)
					@REM Copy other files (packagedependencies.txt, bcom<package>.pc, ...)
					if %%l == static (
						xcopy /y /q !BUILDFOLDER!\packagedependencies-static.txt %PKGROOT%\packagedependencies.txt
					) else (
						xcopy /y /q !BUILDFOLDER!\packagedependencies.txt %PKGROOT%
					)
					xcopy /y /q !BUILDFOLDER!\%PKGNAME%-%PKGVERSION%_remakeninfo.txt %PKGROOT%
					xcopy /y /q !BUILDFOLDER!\remaken-%PKGNAME%.pc %PKGROOT%
					@REM Compress archive
					echo Packaging %PKGNAME%-%PKGVERSION% from folder %PKGROOT%
					7z a -tzip %PKGNAME%_%PKGVERSION%_x86_64_%%l_%%b.zip %PKGROOT% >NUL
					@REM Clean solution
					rmdir /s /q %PKGNAME%
					echo solution clean
				)
			)
		)
	)
)

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

