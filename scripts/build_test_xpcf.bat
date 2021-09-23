@echo off
setlocal

call build_xpcf_shared.bat
call build_testxpcf.bat
REM call build_xpcf_static.bat

:END
cd "%~dp0"

endlocal
goto:eof






