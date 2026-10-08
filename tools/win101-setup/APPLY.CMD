@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET="
echo Windows 1.01 patch preparation
echo.
set /p "TARGET=Target Windows 1.01 installation-media directory: "
if not defined TARGET (
  echo No directory entered.
  pause
  exit /b 64
)
if not exist "%TARGET%\SETUP.EXE" (
  echo SETUP.EXE was not found in "%TARGET%".
  pause
  exit /b 2
)
echo.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0apply-setup.ps1" -MediaRoot "%TARGET%"
set "RESULT=%errorlevel%"
echo.
echo Patch preparation exit code: %RESULT%
pause
exit /b %RESULT%
