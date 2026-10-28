@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
set "OUT=%~1"
if not defined OUT set "OUT=build\t473-s10\tabstop-release-seams"
if not exist "%OUT%" mkdir "%OUT%"
cl /nologo /TC /c /O2 /MT artifacts\mvdm-audit\host-ch-scope\tabstop-release-fixture.c /Fo:"%OUT%\fixture.obj"
if errorlevel 1 exit /b %errorlevel%
cl /nologo /TC /c /O2 /MT artifacts\mvdm-audit\host-ch-scope\tabstop-release-seams.c /Fo:"%OUT%\seams.obj"
if errorlevel 1 exit /b %errorlevel%
link /nologo /out:"%OUT%\fixture.exe" /map:"%OUT%\fixture.map" "%OUT%\fixture.obj" "%OUT%\seams.obj" build\.ninja\msvc-x86-release\wow32\obj\wmsgem.obj kernel32.lib user32.lib
exit /b %errorlevel%
