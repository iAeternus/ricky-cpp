@echo off
setlocal enabledelayedexpansion

set BUILD_DIR=build
set CPU_CORES=16

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
    echo Created build directory: %BUILD_DIR%
) else (
    echo Build directory already exists: %BUILD_DIR%
)

cd "%BUILD_DIR%"

echo Configuring project...
cmake .. -G "MinGW Makefiles"
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to configure project.
    exit /b %ERRORLEVEL%
)
echo Project configured successfully.

echo Compiling project with %CPU_CORES% parallel jobs...
cmake --build . --parallel %CPU_CORES%
if %ERRORLEVEL% NEQ 0 (
    echo Error: Compilation or linking failed.
    exit /b %ERRORLEVEL%
)
echo Project compiled and linked successfully.

endlocal