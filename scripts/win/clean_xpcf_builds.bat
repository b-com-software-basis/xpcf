@echo off
cls
setlocal

for /f "delims=" %%D in ('dir /a:d /b build-*') do (
    echo "Removing %%~fD folder"
    rmdir /S /Q %%~FD
)

endlocal
goto:eof

:end


