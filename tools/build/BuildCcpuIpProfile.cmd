@echo off
if "%~1"=="" exit /b 64
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /W4 "%~1\ip-profile.c" /Fo"%~1\ip-profile.obj" /Fe"%~1\ip-profile.exe"
if errorlevel 1 exit /b %errorlevel%
"%~1\ip-profile.exe"
exit /b %errorlevel%
