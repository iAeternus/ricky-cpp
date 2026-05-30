@echo off
chcp 65001 >nul
REM 运行已构建的 example（不执行构建）。
REM
REM 用法:
REM   run_example.bat                      列出可用 example
REM   run_example.bat regression_mlp -t    运行指定 example 并传参

setlocal enabledelayedexpansion
cd /d "%~dp0"

if "%~1"=="" (
    echo Usage: run_example.bat ^<example_name^> [args...]
    echo.
    echo Available examples:
    dir /b "build\bin\examples\*" 2>nul
    exit /b 0
)

set "EXAMPLE=%~1"
shift

set "EXE=build\bin\examples\%EXAMPLE%"
if not exist "%EXE%" (
    echo Error: '%EXAMPLE%' not found (build\bin\examples\%EXAMPLE% missing)
    echo Run 'cmake --build build --target %EXAMPLE%' first, or build the project.
    exit /b 1
)

"%EXE%" %*
