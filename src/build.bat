@echo off

SETLOCAL
setlocal EnableDelayedExpansion
SET "BUILD_DIR=..\builds"
SET "CC=cl"
SET "CFLAGS=-FC -Zi -Wall"
SET "LIBS=user32.lib gdi32.lib"

SET "SRC_PREFIX=..\src\"
SET __FILES__=blastbeat_main.cpp ^
              input\input.cpp ^
              sound\sound.cpp ^
              rendering\render_buffer.cpp ^
              utility\rect_converter.cpp
FOR %%F IN (%__FILES__%) DO (
        SET "FILES=!FILES! %SRC_PREFIX%%%F"
)

SET __INCLUDE_DIRS__=.\
FOR %%D IN (%__INCLUDE_DIRS__%) DO (
        SET "INCLUDE_DIRS=!INCLUDE_DIRS! /I %SRC_PREFIX%%%D"
)

mkdir %BUILD_DIR%
pushd %BUILD_DIR%
%CC% %INCLUDE_DIRS% %CFLAGS% %FILES% %LIBS%
popd
