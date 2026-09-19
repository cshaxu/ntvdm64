@echo off
if "%~1"=="" exit /b 64
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /W4 "%~1\exception-profile.c" /Fo"%~1\exception-profile.obj" /Fe"%~1\exception-profile.exe"
if errorlevel 1 exit /b %errorlevel%
"%~1\exception-profile.exe"
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /W4 "%~1\exception-prod-environment.c" /Fo"%~1\exception-prod-environment.obj" /Fe"%~1\exception-prod-environment.exe"
if errorlevel 1 exit /b %errorlevel%
"%~1\exception-prod-environment.exe"
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /W4 "%~1\exception-before.c" /Fo"%~1\exception-before.obj" /Fe"%~1\exception-before.exe"
if errorlevel 1 exit /b %errorlevel%
"%~1\exception-before.exe"
if not errorlevel 1 exit /b 1
echo PASS original-code negative control rejected
exit /b 0
