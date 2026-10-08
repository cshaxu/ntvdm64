@echo off
setlocal EnableExtensions
if "%~1"=="" goto usage
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64 >nul || exit /b 1
for %%I in ("%~f1") do (
  set "OUT=%%~fI"
  set "OUTDIR=%%~dpI"
)
pushd "%OUTDIR%" || exit /b 1
cl.exe /nologo /TC /std:c11 /W4 /WX /MT /Fe"%OUT%" "%~dp0grp.c" /link /MACHINE:X64
set "RESULT=%errorlevel%"
popd
exit /b %RESULT%
:usage
echo Usage: BUILD.CMD output-exe
exit /b 64
