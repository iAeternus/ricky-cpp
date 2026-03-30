@echo off
setlocal

if "%~1"=="" (
    echo Usage: run_example.bat [example_name]
    echo Example: run_example.bat tcp_server
    echo.
    echo Available examples:
    dir /b "build\bin\examples\*.exe" 2>nul
    exit /b 1
)

set EXAMPLE=%~1.exe
set EXAMPLE_DIR=build\bin\examples

if not exist "%EXAMPLE_DIR%\%EXAMPLE%" (
    echo Error: Example "%EXAMPLE%" not found
    echo Available examples:
    dir /b "%EXAMPLE_DIR%\*.exe"
    exit /b 1
)

"%EXAMPLE_DIR%\%EXAMPLE%"
