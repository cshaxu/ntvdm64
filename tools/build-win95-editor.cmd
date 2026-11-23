@echo off
setlocal EnableExtensions DisableDelayedExpansion
if not "%~5"=="" exit /b 2
if "%~2"=="" exit /b 2
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
cmake -S "%~dp0build\win31-editor" -B "%ROOT%\build\.ninja\win95-editor" -G Ninja -DEDITOR_PROFILE=win95 "-DEDITOR_DEST=%~2" "-DEDITOR_FEATURES=%~3" "-DEDITOR_WELCOME_HEX=%~4"
if errorlevel 1 exit /b %errorlevel%
cmake --build "%ROOT%\build\.ninja\win95-editor"
if errorlevel 1 exit /b %errorlevel%
copy /b "%ROOT%\build\.ninja\win95-editor\controller.exe" "%OUTPUT%" >nul
exit /b %errorlevel%
