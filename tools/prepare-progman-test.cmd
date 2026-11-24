@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~2"=="" exit /b 2
if not "%~3"=="" exit /b 2
set "stage=%~f1"
set "source=%~f2"
set "invalid="
for /f "delims=abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-" %%I in ("%~nx1") do set "invalid=1"
for %%I in ("%stage%\..") do if /i not "%%~fI"=="O:\tmp" set "invalid=1"
if defined invalid exit /b 2
if not exist "%stage%\.ntvdm64-test-stage.json" exit /b 2
for %%I in (MAIN.GRP ACCESSOR.GRP GAMES.GRP STARTUP.GRP) do (
  if not exist "%source%\%%I" set "invalid=1"
  if exist "%source%\%%I\" set "invalid=1"
)
if defined invalid exit /b 2
for %%I in (MAIN.GRP ACCESSOR.GRP GAMES.GRP STARTUP.GRP) do (
  copy /b "%source%\%%I" "%stage%\%%I" >nul || set "invalid=1"
)
if defined invalid exit /b 1
>"%stage%\PROGMAN.INI" (
  echo [Settings]
  echo Window=64 48 576 384 1
  echo Order= 1 2 3 4
  echo.
  echo [Groups]
  echo Group1=%stage%\MAIN.GRP
  echo Group2=%stage%\ACCESSOR.GRP
  echo Group3=%stage%\GAMES.GRP
  echo Group4=%stage%\STARTUP.GRP
)
exit /b %errorlevel%
