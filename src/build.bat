@echo off

SETLOCAL
SET BUILD_DIR=..\builds
SET CC=cl
SET CFLAGS=-Zi
SET LIBS=user32.lib

mkdir %BUILD_DIR%
pushd %BUILD_DIR%
%CC% %CFLAGS% ..\src\blastbeat_main.cpp %LIBS%
popd
