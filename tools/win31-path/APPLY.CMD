@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET="
echo Windows 3.1 path repair
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
if not exist "%~dp0WIN31PATH.EXE" (
  echo Missing WIN31PATH.EXE beside APPLY.CMD.
  pause
  exit /b 3
)
echo.
"%~dp0WIN31PATH.EXE" --root "%TARGET%"
set "RESULT=%errorlevel%"
echo.
echo Path repair exit code: %RESULT%
pause
exit /b %RESULT%
