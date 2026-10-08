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
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
for %%F in (SETUP.EXE SETUP.LBL KERNEL.EXE USER.EXE GDI.EXE DISK1 DISK2 DISK3 DISK4 DISK5) do if not exist "%TARGET%\%%F" (
  echo Missing original media file: %%F
  pause
  exit /b 2
)
for %%F in (SETUP.CMD README.TXT MOUSE.DRV SETVER.EXE PIF.EXE HASH.EXE) do if not exist "%~dp0%%F" (
  echo Tool package is incomplete: %%F
  pause
  exit /b 3
)
set "PATCH=%TARGET%\PATCH"
if not exist "%PATCH%\" mkdir "%PATCH%" || (
  echo Could not create "%PATCH%".
  pause
  exit /b 4
)
for %%F in (SETUP.CMD README.TXT MOUSE.DRV SETVER.EXE PIF.EXE HASH.EXE) do copy /y "%~dp0%%F" "%PATCH%\%%F" >nul || (
  echo Could not copy %%F.
  pause
  exit /b 5
)
for %%F in (configure-launch.ps1 run-setup.ps1 win.cmd.template WIN31-TEMPLATE.PIF addon-files.json) do if exist "%PATCH%\%%F" del /q "%PATCH%\%%F"
set "RESULT=0"
echo.
echo Patch preparation exit code: %RESULT%
pause
exit /b %RESULT%
