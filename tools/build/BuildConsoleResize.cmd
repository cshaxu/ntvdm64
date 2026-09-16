@echo off
if "%~2"=="" exit /b 64
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /MT /W4 /I src /I src\adapter-mvdm-host-out\win32\include tests\observation\console_resize_contract.c /Fo"%~1\resize-test.obj" /Fe"%~1\resize-test.exe" /link "%~2\softpc-win32-bindings.lib" "%~2\session.lib" user32.lib gdi32.lib advapi32.lib
exit /b %errorlevel%
