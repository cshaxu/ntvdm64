@echo off
setlocal EnableExtensions DisableDelayedExpansion
for %%I in ("%~dp0..") do set "repo=%%~fI"
call "%repo%\tools\build\bootstrap.cmd"
if errorlevel 1 exit /b %errorlevel%
call "%repo%\tools\build-addon.cmd" all
if errorlevel 1 exit /b %errorlevel%
call "%repo%\build\.ninja\addon\publish-utilities.cmd" %*
exit /b %errorlevel%
