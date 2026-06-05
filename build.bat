@echo off
setlocal

set root_dir=%~dp0
set root_dir=%root_dir:~0,-1%
set build_dir=%root_dir%\build

set generator=Ninja

if defined BUILD_TYPE (
    set build_type=%BUILD_TYPE%
) else (
    set build_type=Debug
)

cmake -S "%root_dir%" -B "%build_dir%" -G "%generator%" -DCMAKE_BUILD_TYPE="%build_type%"
if %ERRORLEVEL% NEQ 0 exit /b %ERRORLEVEL%

cmake --build "%build_dir%" --parallel
