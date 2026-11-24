@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~2"=="" exit /b 2
if not "%~4"=="" exit /b 2
if not "%~3"=="" if not "%~3"=="--local" exit /b 2
for %%I in ("%~dp0..") do set "repo=%%~fI"
set "target=%~f1"
set "owner=%~f2"
set "temporary=O:\tmp"
if "%~3"=="--local" set "temporary=%LOCALAPPDATA%\Temp"
set "invalid="
for /f "delims=abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-" %%I in ("%~nx1") do set "invalid=1"
for %%I in ("%target%\..") do if /i not "%%~fI"=="%temporary%" set "invalid=1"
if defined invalid exit /b 2
if exist "%target%" exit /b 2
if not exist "%owner%\" exit /b 2
set "walk=%owner%"
:within
for %%I in ("%walk%\..") do set "parent=%%~fI"
if /i "%parent%"=="%repo%\build" goto create
if /i "%parent%"=="%walk%" exit /b 2
set "walk=%parent%"
goto within
:create
mkdir "%target%" || exit /b 1
<nul set /p "=%owner%">"%target%\.ntvdm64-guest-owner"
if not exist "%target%\.ntvdm64-guest-owner" exit /b 1
exit /b 0
