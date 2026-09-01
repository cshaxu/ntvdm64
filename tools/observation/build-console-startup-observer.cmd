@echo off
setlocal
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
if not exist build\tools mkdir build\tools
cl.exe /nologo /W4 /MT /Fo:build\tools\console-startup-observer.obj /Fe:build\tools\console-startup-observer.exe tests\observation\console_startup_observer.c dbghelp.lib
