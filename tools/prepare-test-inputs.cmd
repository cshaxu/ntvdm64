@echo off
setlocal EnableExtensions DisableDelayedExpansion
call "%~dp0build\bootstrap.cmd" >nul
if errorlevel 1 exit /b %errorlevel%
call "%~dp0..\build\.ninja\build-tools\prepare-test-inputs.cmd" %*
exit /b %errorlevel%
