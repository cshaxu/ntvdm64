@echo off
setlocal EnableExtensions DisableDelayedExpansion
if not "%~8"=="" exit /b 2
if "%~7"=="" exit /b 2
for %%I in (%3 %4 %5 %6 %7) do if not "%%I"=="0" if not "%%I"=="1" exit /b 2
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
cmake -S "%~dp0build\win31-editor" -B "%ROOT%\build\.ninja\win31-editor" -G Ninja "-DEDITOR_DEST=%~2" -DMOUSE_MENU=%3 -DDOSONLY=%4 -DSAVE_WITNESS=%5 -DFIRST_DOS=%6 -DFIRST_DOS_BEFORE_SECOND=%7
if errorlevel 1 exit /b %errorlevel%
cmake --build "%ROOT%\build\.ninja\win31-editor"
if errorlevel 1 exit /b %errorlevel%
copy /b "%ROOT%\build\.ninja\win31-editor\controller.exe" "%OUTPUT%" >nul
exit /b %errorlevel%
