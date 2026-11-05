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
>"%WINDOWS%\SYSTEM.INI" echo [boot]
>>"%WINDOWS%\SYSTEM.INI" echo mouse.drv=MOUSE.DRV
echo %WINDOWS%| call "%~dp0..\..\tools\win95-launch\UNAPPLY.CMD" || exit /b 4
findstr /x /c:"original-vmm" "%SYSTEM%\VMM32.VXD" >nul || exit /b 5
findstr /x /c:"original-mouse" "%SYSTEM%\MOUSE.DRV" >nul || exit /b 6
if exist "%SYSTEM%\VMM32.VXD.W95LAUNCH.BAK" exit /b 7
if exist "%SYSTEM%\MOUSE.DRV.W95LAUNCH.BAK" exit /b 8
if exist "%PATCH%\" exit /b 9
echo WIN95_LAUNCH_UNAPPLY_OK
exit /b 0
