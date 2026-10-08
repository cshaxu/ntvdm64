@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET="
echo Windows 3.1 NTVDM launch preparation
echo.
set /p "TARGET=Target installed Windows 3.1 directory: "
if not defined TARGET (
  echo No directory entered.
  pause
  exit /b 64
)
if not exist "%TARGET%\WIN.COM" (
  echo WIN.COM was not found in "%TARGET%".
  pause
  exit /b 2
)
if not exist "%TARGET%\SYSTEM\KRNL386.EXE" (
  echo SYSTEM\KRNL386.EXE was not found in "%TARGET%".
  pause
  exit /b 2
)
if not exist "%~dp0WIN31LAUNCH.EXE" (
  echo Missing WIN31LAUNCH.EXE beside APPLY.CMD.
  pause
  exit /b 3
)
if not exist "%~dp0MOUSE31.DRV" (
  echo Missing MOUSE31.DRV beside APPLY.CMD.
  pause
  exit /b 3
)
echo.
"%~dp0WIN31LAUNCH.EXE" --root "%TARGET%" --driver "%~dp0MOUSE31.DRV"
set "RESULT=%errorlevel%"
echo.
echo Launch preparation exit code: %RESULT%
pause
exit /b %RESULT%
