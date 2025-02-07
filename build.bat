@echo off
setlocal enabledelayedexpansion

set BUILD_DIR=build
set CPU_CORES=16
set GENERATOR="MinGW Makefiles"

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
    echo Created build directory: %BUILD_DIR%
) else (
    echo Build directory already exists: %BUILD_DIR%
)

cd "%BUILD_DIR%"

if exist CMakeCache.txt (
    echo Checking existing CMake configuration...
    setlocal
    set "OLD_GENERATOR="
    
    for /f "tokens=1,* delims== " %%A in ('findstr /b /c:"CMAKE_GENERATOR:INTERNAL" CMakeCache.txt') do (
        set "OLD_GENERATOR=%%B"
    )
    
    endlocal & set "OLD_GENERATOR=%OLD_GENERATOR%"
    
    if "!OLD_GENERATOR!" NEQ %GENERATOR% (
        echo Detected different generator: [!OLD_GENERATOR%]
        echo Cleaning old CMake configuration...
        del CMakeCache.txt
        rmdir /s /q CMakeFiles
        echo Old configuration cleaned for generator switch
    )
)

echo Configuring project with %GENERATOR%...
cmake .. -G %GENERATOR%
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to configure project.
    exit /b %ERRORLEVEL%
)
echo Project configured successfully.

echo Compiling project with %CPU_CORES% parallel jobs...
cmake --build . -j %CPU_CORES%
if %ERRORLEVEL% NEQ 0 (
    echo Error: Compilation or linking failed.
    exit /b %ERRORLEVEL%
)
echo Project compiled and linked successfully.

endlocal