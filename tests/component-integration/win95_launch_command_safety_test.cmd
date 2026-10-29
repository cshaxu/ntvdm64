@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "ROOT=%~1"
if not defined ROOT exit /b 64
for %%I in ("%ROOT%") do set "ROOT=%%~fI"
set "FIXTURE=%ROOT%\WINDOWS"
if exist "%ROOT%" exit /b 64
mkdir "%FIXTURE%\SYSTEM" "%FIXTURE%\PATCH" || exit /b 2
>"%FIXTURE%\WIN.COM" echo fixture
>"%FIXTURE%\SYSTEM\VMM32.VXD" echo fixture-vmm
>"%FIXTURE%\SYSTEM\KRNL386.EXE" echo fixture-kernel
>"%FIXTURE%\SYSTEM\MOUSE.DRV" echo fixture-mouse
>"%FIXTURE%\SYSTEM.INI" echo [boot]
>>"%FIXTURE%\SYSTEM.INI" echo mouse.drv=MOUSE.DRV
>"%FIXTURE%\PATCH\WIN.CMD" echo owner content

call "%~dp0..\..\tools\win95-launch\APPLY.CMD" "%FIXTURE%"
if not errorlevel 1 exit /b 3
findstr /x /c:"owner content" "%FIXTURE%\PATCH\WIN.CMD" >nul || exit /b 4
call "%~dp0..\..\tools\win95-launch\UNAPPLY.CMD" "%FIXTURE%"
if not errorlevel 1 exit /b 5
findstr /x /c:"owner content" "%FIXTURE%\PATCH\WIN.CMD" >nul || exit /b 6
echo WIN95_LAUNCH_COMMAND_SAFETY_OK
exit /b 0
