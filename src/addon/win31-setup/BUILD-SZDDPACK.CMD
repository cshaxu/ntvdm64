@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~2"=="" goto :usage
set "OUTPUT=%~1"
set "BUILDROOT=%~2"
if not exist "%~dp0szddpack.c" echo Missing szddpack.c& exit /b 2
if not exist "%BUILDROOT%" mkdir "%BUILDROOT%" || exit /b 3
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul || exit /b 4
cl.exe /nologo /c /TC /std:c11 /W4 /WX /MT /Fo"%BUILDROOT%\szddpack.obj" "%~dp0szddpack.c" || exit /b 5
link.exe /nologo /MACHINE:X64 /OUT:"%OUTPUT%" "%BUILDROOT%\szddpack.obj" kernel32.lib || exit /b 6
exit /b 0
:usage
echo Usage: BUILD-SZDDPACK.CMD OUTPUT-EXE BUILD-DIRECTORY
exit /b 64
