@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64 >nul
if errorlevel 1 exit /b %errorlevel%
set "PROBE_OUT=%~1"
if not defined PROBE_OUT set "PROBE_OUT=build\t473-s10\winmm-real-null"
if not exist "%PROBE_OUT%" mkdir "%PROBE_OUT%"
if errorlevel 1 exit /b %errorlevel%
cl /nologo /TC /W3 /O2 /MT artifacts\mvdm-audit\host-ch-scope\wow-winmm-real-null-fixture.c /Fo:"%PROBE_OUT%\fixture.obj" /Fe:"%PROBE_OUT%\fixture.exe"
if errorlevel 1 exit /b %errorlevel%
cl /nologo /TC /c /W3 /O2 /Gy /MT /DTEST_WOW_WRAPPER artifacts\mvdm-audit\host-ch-scope\wow-winmm-real-null-fixture.c /Fo:"%PROBE_OUT%\wrapper.obj"
if errorlevel 1 exit /b %errorlevel%
link /nologo /OPT:REF /out:"%PROBE_OUT%\wrapper.exe" /map:"%PROBE_OUT%\wrapper.map" "%PROBE_OUT%\wrapper.obj" build\.ninja\msvc-x86-release\wow32\obj\wmmedia.obj kernel32.lib
exit /b %errorlevel%
