@echo off

set "BUILD_DIR=build"

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

echo Start Build Project
cd "%BUILD_DIR%"
cmake .. -G "MinGW Makefiles"

echo Start Compile Project
make && echo Compile and Link Successfully || (echo Compilation or Linking Failed & exit /b %ERRORLEVEL%)