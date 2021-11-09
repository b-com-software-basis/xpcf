@echo off
cls
setlocal
set rootDir=%~dp0

call buildzlibWin.bat debug
call buildzlibWin.bat release
call package_zlib.bat
call install_zlib.bat

call buildGRPCWin.bat debug
call buildGRPCWin.bat release
call package_grpc.bat
call install_grpc.bat
