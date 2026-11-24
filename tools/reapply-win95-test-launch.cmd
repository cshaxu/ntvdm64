@echo off
rem Restore the launcher's pre-apply layout on an owned test copy.
rem Arguments: INSTALL OWNER PACKAGE DOSONLY MULTITASK
setlocal EnableExtensions DisableDelayedExpansion
if not "%~6"=="" exit /b 2
for %%V in ("%~4" "%~5") do if not "%%~V"=="0" if not "%%~V"=="1" exit /b 2
set "DEST=%~f1"
set "TASK=%~f2"
set "PACKAGE=%~f3"
for %%I in ("%DEST%\..") do if /i not "%%~fI"=="O:\tmp" exit /b 2
if not exist "%DEST%\.ntvdm64-guest-owner" exit /b 2
set "OWNER="
set /p "OWNER="<"%DEST%\.ntvdm64-guest-owner"
if /i not "%OWNER%"=="%TASK%" exit /b 2
if not exist "%TASK%\" exit /b 2
if not exist "%PACKAGE%\system32\run16.exe" exit /b 2
if exist "%TASK%\launch-input.txt" exit /b 2
set "PATCH=%DEST%\PATCH"
if not exist "%PATCH%\" exit /b 2
copy /b /y "%~dp0win95-launch\WIN.CMD" "%PATCH%\WIN.CMD" >nul || exit /b 1
call "%PATCH%\WIN.CMD" --remove >"%TASK%\launch-reset.log" 2>&1
if errorlevel 1 exit /b 1
set "FAILED="
for %%F in (PIF.EXE CONFIG.NT AUTOEXEC.NT WIN.PIF WIN.CMD compat95.sys SETVER.EXE INSIGNIA.386 NTVDM64.386 MULTITASK.INI.BAK MULTITASK.INI.NEW MULTITASK.HASH MULTITASK.LAUNCH) do if exist "%PATCH%\%%F" (
 if exist "%PATCH%\%%F\" exit /b 2
 del /q "%PATCH%\%%F"
 if errorlevel 1 set "FAILED=1"
)
if defined FAILED exit /b 1
if exist "%PATCH%\TEMP\" rmdir "%PATCH%\TEMP" || exit /b 1
if exist "%DEST%\SYSTEM\MOUSE.DRV.BAK\" exit /b 2
if exist "%DEST%\SYSTEM\MOUSE.DRV.BAK" del /q "%DEST%\SYSTEM\MOUSE.DRV.BAK" || exit /b 1
>"%TASK%\launch-input.txt" echo %DEST%
if errorlevel 1 exit /b 1
set "PATH=%PACKAGE%\system32;%PATH%"
call "%~dp0win95-launch\APPLY.CMD" <"%TASK%\launch-input.txt" >"%TASK%\launch-apply.log" 2>&1
if errorlevel 1 exit /b 1
call "%~dp0select-guest-launch.cmd" "%DEST%" "%TASK%" 95 %4 %5 select
exit /b %errorlevel%
