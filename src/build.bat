@echo off

SETLOCAL
setlocal EnableDelayedExpansion
SET "BUILD_DIR=..\builds"
SET "CC=cl"
SET "CFLAGS=-FC -Zi"
SET "LIBS=user32.lib gdi32.lib"

SET "SRC_PREFIX=..\src\"
SET __FILES__=blastbeat_main.cpp ^
              utility\rect_converter.cpp
FOR %%F IN (%__FILES__%) DO (
        SET "FILES=!FILES! %SRC_PREFIX%%%F"
)

mkdir %BUILD_DIR%
pushd %BUILD_DIR%
%CC% %CFLAGS% %FILES% %LIBS%
popd
