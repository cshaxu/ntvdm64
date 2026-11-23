@echo off
setlocal EnableExtensions DisableDelayedExpansion
if not "%~4"=="" exit /b 2
if not "%~2"=="dos" if not "%~2"=="retirement" if not "%~2"=="reuse" if not "%~2"=="controller" exit /b 2
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
cmake -S "%~dp0build\vm-identity" -B "%ROOT%\build\.ninja\vm-identity\%~2" -G Ninja "-DIDENTITY_KIND=%~2" "-DIDENTITY_DEFINES=%~3"
if errorlevel 1 exit /b %errorlevel%
cmake --build "%ROOT%\build\.ninja\vm-identity\%~2"
if errorlevel 1 exit /b %errorlevel%
copy /b "%ROOT%\build\.ninja\vm-identity\%~2\probe.bin" "%OUTPUT%" >nul
exit /b %errorlevel%
