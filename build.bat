@echo off

set "BUILD_DIR=build"

if exist "%BUILD_DIR%" (
    rmdir /S /Q "%BUILD_DIR%"
)
mkdir "%BUILD_DIR%"

:: 构建工程
echo Start build project.
cd "%BUILD_DIR%"
cmake .. -G "MinGW Makefiles"

:: 编译项目
echo Start compile project.
make

:: 编译测试项目
set "TEST_DIR=tests"
if not exist "%TEST_DIR%" (
    echo The directory "%TEST_DIR%" does not exist.
) else (
    cd "%TEST_DIR%"
    echo Start compile test project.
    make
)