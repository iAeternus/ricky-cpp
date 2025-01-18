@echo off

set "BUILD_DIR=build"

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

echo Start Build Project
cd "%BUILD_DIR%"
cmake .. -G "MinGW Makefiles"

echo Start Compile Project
make && echo Compilation and Linking Successfully || (echo Compilation or Linking Failed & exit /b %ERRORLEVEL%)