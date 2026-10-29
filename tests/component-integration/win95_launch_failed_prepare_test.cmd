@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "ROOT=%~1"
if not defined ROOT exit /b 64
for %%I in ("%ROOT%") do set "ROOT=%%~fI"
if exist "%ROOT%" exit /b 64
set "WINDOWS=%ROOT%\WINDOWS"
mkdir "%WINDOWS%\SYSTEM" || exit /b 2
>"%WINDOWS%\WIN.COM" echo fixture
>"%WINDOWS%\SYSTEM\VMM32.VXD" echo unsupported fixture
>"%WINDOWS%\SYSTEM\KRNL386.EXE" echo fixture-kernel
>"%WINDOWS%\SYSTEM\MOUSE.DRV" echo original mouse
>"%WINDOWS%\SYSTEM.INI" echo [boot]
>>"%WINDOWS%\SYSTEM.INI" echo mouse.drv=MOUSE.DRV
call "%~dp0..\..\tools\win95-launch\APPLY.CMD" "%WINDOWS%" >"%ROOT%\apply.log" 2>&1
if not errorlevel 1 exit /b 3
findstr /c:"unsupported or malformed VMM32" "%ROOT%\apply.log" >nul || exit /b 9
if exist "%WINDOWS%\SYSTEM\KRNL386.EXE.W95LAUNCH.BAK" exit /b 10
if exist "%WINDOWS%\SYSTEM\VMM32.VXD.W95LAUNCH.BAK" exit /b 4
if exist "%WINDOWS%\SYSTEM\MOUSE.DRV.W95LAUNCH.BAK" exit /b 5
if exist "%WINDOWS%\PATCH" exit /b 6
findstr /x /c:"unsupported fixture" "%WINDOWS%\SYSTEM\VMM32.VXD" >nul || exit /b 7
findstr /x /c:"original mouse" "%WINDOWS%\SYSTEM\MOUSE.DRV" >nul || exit /b 8
echo WIN95_LAUNCH_FAILED_PREPARE_ROLLBACK_OK
exit /b 0
