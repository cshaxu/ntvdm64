@echo off
setlocal EnableExtensions
if "%~1"=="" exit /b 64
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul || exit /b 1
for %%I in ("%~f1") do set "OUT=%%~fI"
for %%I in ("%~f1") do set "OUTDIR=%%~dpI"
pushd "%OUTDIR%" || exit /b 1
cl.exe /nologo /TC /std:c11 /W4 /WX /MT /Fe"%OUT%" "%~dp0lnk.c" /link /MACHINE:X64
set "RESULT=%errorlevel%"
popd
exit /b %RESULT%
