@echo off
setlocal EnableExtensions
if "%~1"=="" goto usage
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul || exit /b 1
cl.exe /nologo /TC /std:c11 /W4 /WX /MT /DWINNT /I "%~dp0..\..\opennt-abi\source\public\internal\windows\inc" /Fe"%~1" "%~dp0pif.c" /link /MACHINE:X64 || exit /b 1
exit /b 0
:usage
echo Usage: BUILD.CMD output-exe
exit /b 64
