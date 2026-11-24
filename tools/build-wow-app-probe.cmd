@echo off
setlocal EnableExtensions DisableDelayedExpansion
if not "%~3"=="" exit /b 2
if "%~2"=="" exit /b 2
set "SELECTED="
for %%I in (sol-gameplay sol-options write-lifecycle) do if "%~1"=="%%I" set "SELECTED=1"
if not defined SELECTED exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "OUTPUT=%~f2"
if not exist "%OUTPUT%\" exit /b 2
if exist "%OUTPUT%\probe.exe" exit /b 2
set "PARENT=%OUTPUT%"
:check_parent
set "PREVIOUS=%PARENT%"
for %%I in ("%PARENT%\..") do set "PARENT=%%~fI"
if /i "%PARENT%"=="%ROOT%\build" goto build
if /i "%PARENT%"=="%PREVIOUS%" exit /b 2
goto check_parent
:build
call "%~dp0build-fixtures.cmd" x86 wow-%~1
if errorlevel 1 exit /b %errorlevel%
copy /b "%ROOT%\build\.ninja\fixtures-x86\wow-%~1.exe" "%OUTPUT%\probe.exe" >nul
exit /b %errorlevel%
