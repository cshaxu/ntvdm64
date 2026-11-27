@echo off
setlocal EnableExtensions DisableDelayedExpansion
for %%I in ("%~dp0..\..\..\..\..") do set "repo=%%~fI"
if "%~1"=="" exit /b 2
set "out=%~f1"
if not exist "%out%" mkdir "%out%"
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /W4 /DUNICODE /D_UNICODE /I "%repo%\src\non-mvdm\public\internal\net\inc" /Fo"%out%\conversion.obj" /Fe"%out%\conversion.exe" "%~dp0conversion.c" "%repo%\build\.ninja\msvc-x86-release\worker\original-opennt-xactsrv.lib" "%repo%\build\.ninja\msvc-x86-release\worker\original-opennt-netlib.lib" kernel32.lib ntdll.lib netapi32.lib /link /incremental:no > "%out%\build.log" 2>&1
if errorlevel 1 exit /b %errorlevel%
"%out%\conversion.exe" > "%out%\result.txt" 2>&1
exit /b %errorlevel%
