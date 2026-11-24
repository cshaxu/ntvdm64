@echo off
setlocal EnableExtensions DisableDelayedExpansion
for %%I in ("%~dp0..") do set "NTVDM_ROOT=%%~fI"
call "%NTVDM_ROOT%\tools\build\bootstrap.cmd" >nul
if errorlevel 1 exit /b %errorlevel%
call "%NTVDM_ROOT%\tools\build-addon.cmd" hash >nul
if errorlevel 1 exit /b %errorlevel%
call "%NTVDM_ROOT%\build\.ninja\build-tools\stage-system32.cmd" %*
exit /b %errorlevel%
