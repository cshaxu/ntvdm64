@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~1"=="" goto usage
if not "%~2"=="" goto usage
for %%I in ("%~dp0..\..\..") do set "NTVDM_ROOT=%%~fI"
for %%I in ("%~f1") do set "OUTPUT=%%~fI"
call "%NTVDM_ROOT%\tools\build-addon.cmd" lnk
if errorlevel 1 exit /b %errorlevel%
copy /b /y "%NTVDM_ROOT%\build\.ninja\addon\lnk\lnk.exe" "%OUTPUT%" >nul
exit /b %errorlevel%
:usage
echo Usage: BUILD.CMD output-exe
exit /b 64
