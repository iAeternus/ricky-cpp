@echo off

set "BUILD_DIR=build"

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

echo Start Build Project
cd "%BUILD_DIR%"
cmake .. -G "MinGW Makefiles"

echo Start Compile Project
make
echo Compiled Successfully