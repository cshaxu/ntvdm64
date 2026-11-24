@echo off
setlocal EnableExtensions DisableDelayedExpansion
call "%~dp0build\bootstrap.cmd" >nul
if errorlevel 1 exit /b %errorlevel%
call "%~dp0..\build\.ninja\build-tools\prepare-wow-diagnostic.cmd" %*
exit /b %errorlevel%
