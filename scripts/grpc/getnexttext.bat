@echo off
REM cls
setlocal

REM check whether user had supplied -h or --help . If yes display usage 
for %%A in ("--help" "-h") do if "%1"==%%A (call:display_usage & exit /b 0)

if "%2" == "" goto args_count_ko
if "%3" == "" goto args_count_ok
:args_count_ko
call:display_usage
goto end
:args_count_ok

set file=%1
set search_for=%2
REM echo %file%
REM echo %search_for%

for /f "tokens=1 delims=:" %%# in ('findstr /n /c:%search_for% %file%') do (
    set "line=%%#"	
    goto :break
)
:break

REM echo %line%

for /f "skip=%line% delims=" %%b in (%file%) do (
    set "nexttext=%%b"
    goto :break2
)
:break2

echo %nexttext%

goto end
endlocal 

::--------------------------------------------------------
::-- Function display_usage starts below here
::--------------------------------------------------------

:display_usage

echo search the next line in a file according to a current line text.
echo It expects two arguments. 
echo.
echo Usage: params [file to search] [text line to search]
REM exit /b 0

:end