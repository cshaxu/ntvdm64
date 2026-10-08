@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~3"=="" goto :usage
set "PIF_TEMPLATE=%~1"
set "OUTPUT=%~2"
set "BUILDROOT=%~3"
if not exist "%PIF_TEMPLATE%" echo Missing PIF template& exit /b 2
if not exist "%~dp0patchset.c" echo Missing patchset.c& exit /b 2
if not exist "%BUILDROOT%" mkdir "%BUILDROOT%" || exit /b 3
set "RC=%BUILDROOT%\patchset.rc"
rem RC string literals treat backslash as an escape.  Use a portable slash
rem form for the resource filename; Win32 accepts it while the original path
rem remains unchanged for all filesystem checks above.
set "PIF_TEMPLATE_RC=%PIF_TEMPLATE:\=/%"
>"%RC%" echo PIF_TEMPLATE RCDATA "%PIF_TEMPLATE_RC%"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul || exit /b 4
rc.exe /nologo /fo "%BUILDROOT%\patchset.res" "%RC%" || exit /b 5
cl.exe /nologo /c /TC /std:c11 /W4 /WX /MT /DUNICODE /D_UNICODE /Fo"%BUILDROOT%\patchset.obj" "%~dp0patchset.c" || exit /b 6
link.exe /nologo /MACHINE:X64 /OUT:"%OUTPUT%" "%BUILDROOT%\patchset.obj" "%BUILDROOT%\patchset.res" kernel32.lib || exit /b 7
exit /b 0
:usage
echo Usage: BUILD.CMD PIF-TEMPLATE OUTPUT-EXE BUILD-DIRECTORY
exit /b 64
