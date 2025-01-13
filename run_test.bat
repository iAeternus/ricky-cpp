@echo off

set "DIR_NAME=build/tests"
set "EXE_NAME=RICKY_CPP_TESTS"

if not exist "%DIR_NAME%" (
    echo The directory "%DIR_NAME%" does not exist.
) else (
    cd "%DIR_NAME%"
    if exist "%EXE_NAME%.exe" (
        echo Start Running The Test Cases
        .\"%EXE_NAME%".exe
    ) else (
        echo The executable "%EXE_NAME%.exe" was not found in the "%DIR_NAME%" directory.
    )
)