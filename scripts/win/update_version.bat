@echo off

setlocal enableextensions enabledelayedexpansion

set forceReplacement=false
set foundMarker=false
set foundReplace=false
set foundTemplate=false
set help=false
set clean=false
set marker=version
set replace=0.0.0
set filename=

@REM Check parameters
set argC=0
for %%x in (%*) do (
    if "!foundReplace!"=="true" (
        set replace=%%x
        set foundReplace=false
    )
    if "!foundTemplate!"=="true" (
		set filename=%%x
		REM echo !filename!
    )
    if "%%x"=="-f" (
        set forceReplacement=true
    )
    if "%%x"=="--force" (
        set help=true
    )
    if "%%x"=="-h" (
        set help=true
    )
    if "%%x"=="--help" (
        set help=true
    )
    if "%%x"=="-v" (
        set foundReplace=true
    )
    if "%%x"=="--version" (
        set foundReplace=true
    )
    if "%%x"=="--clean" (
        set clean=true
    )
    if "%%x"=="-c" (
        set clean=true
    )
    if "%%x"=="-t" (
		set foundTemplate=true
    )
    if "%%x"=="--template" (
		set foundTemplate=true
    )
)

@REM Check optional parameters
if "!help!"=="true" goto display_usage

if "!foundTemplate!"=="true" (
    if "!filename!"=="" (
        echo "error in template parameters: no template file found"
    ) else (
		set targetFile=!filename:.template=!
		REM echo !targetFile!
        call :generate_file
    )
) else (
	for /f %%f in ('dir /s/b *.template') do (
        
        @REM Define output file
        set filename=%%f
        set targetFile=!filename:.template=!
		REM echo SLE !targetFile!

        @REM Remove generated files
        if "!clean!"=="true" (
            del !targetFile!
            echo removed file !targetFile!
        ) else (
            call :generate_file
        )
    )
)
goto end

:display_usage
echo This script generates or replaces files from all template files (*.template) in the directory and its subdirectory
echo replacing marker "<version>" by the version (default version "0.0.0").
echo. 
echo update_version.bat [options] --version/-v ^<version_number^>
echo options:
echo    -   --help or -h            display usage
echo    -   --force or -f           force generation (replace previous file)
echo    -   --clean or -c           remove all generated files from template
echo    -   --template or -t        generate the file only for the template file given in parameter
exit /b 0

:generate_file
@REM targetFile must be already defined

@REM Check if a new output file must be created
set doReplace=false
if not exist !targetFile! set doReplace=true
if "%forceReplacement%"=="true" set doReplace=true

if "!doReplace!"=="true" (
    @REM Delete older file
    if exist !targetFile! del !targetFile!

    @REM Copy each line replacing searched marker by its value if necessary
    for /f "delims=" %%i in ('type "!filename!"') do (
        set line=%%i
        >> !targetFile! echo(!line:^<%marker%^>=%replace%!
    )
    echo file !targetFile! generated
) else (
    echo no replacement of !targetFile!
)
exit /b 0

:end