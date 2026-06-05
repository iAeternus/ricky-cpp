@echo off
setlocal

set root_dir=%~dp0
set root_dir=%root_dir:~0,-1%
set build_dir=%root_dir%\build

if not exist "%build_dir%\CMakeCache.txt" (
    call "%root_dir%build.bat"
)

"%build_dir%\bin\tests\RICKY_CPP_TESTS.exe" %*
