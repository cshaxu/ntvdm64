@echo off
rem Test artifacts only; caller supplies a task-owned build directory.
if "%~1"=="" exit /b 64
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /W4 tests\observation\console_terminal_observer.c /Fo"%~1\terminal-observer.obj" /Fe"%~1\terminal-observer.exe"
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /W4 tests\observation\console_startup_observer.c /Fo"%~1\observer.obj" /Fe"%~1\observer.exe" /link dbghelp.lib user32.lib
exit /b %errorlevel%
