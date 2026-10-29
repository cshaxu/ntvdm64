@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "ROOT=%~1"
if not defined ROOT exit /b 64
for %%I in ("%ROOT%") do set "ROOT=%%~fI"
if exist "%ROOT%" exit /b 64
set "WINDOWS=%ROOT%\WINDOWS"
set "SYSTEM=%WINDOWS%\SYSTEM"
set "PATCH=%WINDOWS%\PATCH"
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
mkdir "%SYSTEM%" "%PATCH%\TEMP" || exit /b 2
>"%SYSTEM%\VMM32.VXD" echo installed-vmm
>"%SYSTEM%\VMM32.VXD.W95LAUNCH.BAK" echo original-vmm
>"%SYSTEM%\MOUSE.DRV.W95LAUNCH.BAK" echo original-mouse
copy /y "%RELEASE%\MOUSE31.DRV" "%SYSTEM%\MOUSE.DRV" >nul || exit /b 2
copy /y "%RELEASE%\compat95.sys" "%PATCH%\compat95.sys" >nul || exit /b 2
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%SYSTEM%\MOUSE.DRV"') do set "MOUSE_HASH=%%H"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%SYSTEM%\VMM32.VXD"') do set "VMM_HASH=%%H"
if not defined MOUSE_HASH exit /b 3
if not defined VMM_HASH exit /b 3
>"%PATCH%\W95LAUNCH.MANIFEST" (
  echo schema=1
  echo mouse=%SYSTEM%\MOUSE.DRV
  echo mousehash=%MOUSE_HASH%
  echo vmm=%SYSTEM%\VMM32.VXD
  echo vmmhash=%VMM_HASH%
)
call "%~dp0..\..\tools\win95-launch\UNAPPLY.CMD" "%WINDOWS%" || exit /b 4
findstr /x /c:"original-vmm" "%SYSTEM%\VMM32.VXD" >nul || exit /b 5
findstr /x /c:"original-mouse" "%SYSTEM%\MOUSE.DRV" >nul || exit /b 6
if exist "%SYSTEM%\VMM32.VXD.W95LAUNCH.BAK" exit /b 7
if exist "%SYSTEM%\MOUSE.DRV.W95LAUNCH.BAK" exit /b 8
if exist "%PATCH%\W95LAUNCH.MANIFEST" exit /b 9
echo WIN95_LAUNCH_UNAPPLY_OK
exit /b 0
