@echo off
setlocal enabledelayedexpansion

set TEST_ARTIFACTS=build/bin/tests
set TEST_BINARY=RICKY_CPP_TESTS

if not exist "%TEST_ARTIFACTS%" (
    echo Error: The directory "%TEST_ARTIFACTS%" does not exist.
    exit /b 1
)

if not exist "%TEST_ARTIFACTS%\%TEST_BINARY%.exe" (
    echo Error: The executable "%TEST_ARTIFACTS%\%TEST_BINARY%.exe" was not found.
    exit /b 1
)

echo Running tests...
cd "%TEST_ARTIFACTS%"
"%TEST_BINARY%.exe"
if %ERRORLEVEL% NEQ 0 (
    echo Error: Tests failed.
    exit /b %ERRORLEVEL%
)
echo Tests ran successfully.
endlocal