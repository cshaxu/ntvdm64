@echo off
setlocal EnableExtensions DisableDelayedExpansion
if not "%~3"=="" exit /b 2
if not "%~2"=="Empty" if not "%~2"=="WfwNet" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "STAGE=%~f1"
for %%I in ("%STAGE%\..") do if /i not "%%~fI"=="O:\tmp" exit /b 2
if not exist "%STAGE%\.ntvdm64-test-stage.json" exit /b 2
if not exist "%STAGE%\system.ini" exit /b 2
set "TOOL=%ROOT%\build\.ninja\fixtures-x64\wow-network-profile.exe"
if not exist "%TOOL%" exit /b 2
if "%~2"=="Empty" goto configure
set "HASH="
for /f "delims=" %%H in ('call "%ROOT%\assets\release\HASH.EXE" "%STAGE%\system\WFWNET.DRV"') do set "HASH=%%H"
if /i not "%HASH%"=="4C321D43511F845EC1A95518507133D1CF4CF3993BAE422037DBA95CE6258CCA" exit /b 2
:configure
"%TOOL%" "%STAGE%\system.ini" "%~2"
exit /b %errorlevel%
