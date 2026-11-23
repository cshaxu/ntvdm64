@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~2"=="" exit /b 2
if not "%~3"=="" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "OUTPUT=%~f1"
if exist "%OUTPUT%" exit /b 2
set "PARENT=%OUTPUT%"
:check_parent
set "PREVIOUS=%PARENT%"
for %%I in ("%PARENT%\..") do set "PARENT=%%~fI"
if /i "%PARENT%"=="%ROOT%\build" goto build
if /i "%PARENT%"=="%PREVIOUS%" exit /b 2
goto check_parent
:build
cmake -S "%~dp0build\dos-identity" -B "%ROOT%\build\.ninja\dos-identity" -G Ninja "-DDOS_IDENTITY_DEST=%~2"
if errorlevel 1 exit /b %errorlevel%
cmake --build "%ROOT%\build\.ninja\dos-identity"
if errorlevel 1 exit /b %errorlevel%
mkdir "%OUTPUT%"
if errorlevel 1 exit /b %errorlevel%
for %%F in (controller.exe A.COM B.COM) do (
  copy /b "%ROOT%\build\.ninja\dos-identity\%%F" "%OUTPUT%\%%F" >nul
  if errorlevel 1 exit /b 1
)
exit /b 0
