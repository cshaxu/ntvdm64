@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
set "PROBE_OUT=%~1"
if not defined PROBE_OUT set "PROBE_OUT=build\t473-s10\winmm-resolve-object"
if not exist "%PROBE_OUT%" mkdir "%PROBE_OUT%"
if errorlevel 1 exit /b %errorlevel%
objcopy --redefine-sym __imp__LoadLibraryA@4=_fake_imp_LoadLibraryA --redefine-sym __imp__GetProcAddress@8=_fake_imp_GetProcAddress build\.ninja\msvc-x86-release\wow32\obj\wmmedia.obj "%PROBE_OUT%\wmmedia-seam.obj"
if errorlevel 1 exit /b %errorlevel%
cl /nologo /TC /c /W3 /O2 /Gy /MT artifacts\mvdm-audit\host-ch-scope\wow-winmm-resolve-object-fixture.c /Fo:"%PROBE_OUT%\fixture.obj"
if errorlevel 1 exit /b %errorlevel%
link /nologo /OPT:REF /out:"%PROBE_OUT%\fixture.exe" /map:"%PROBE_OUT%\fixture.map" "%PROBE_OUT%\fixture.obj" "%PROBE_OUT%\wmmedia-seam.obj" kernel32.lib
exit /b %errorlevel%
