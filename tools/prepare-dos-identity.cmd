@echo off
rem Owned guest copy, task directory, DOSONLY. Only outputs from authored fixtures are copied.
setlocal EnableExtensions DisableDelayedExpansion
if not "%~4"=="" exit /b 2
if not "%~3"=="0" if not "%~3"=="1" exit /b 2
set "DEST=%~f1"
set "TASK=%~f2"
for %%I in ("%DEST%\..") do if /i not "%%~fI"=="O:\tmp" exit /b 2
if not exist "%DEST%\.ntvdm64-guest-owner" exit /b 2
set "OWNER="
set /p "OWNER="<"%DEST%\.ntvdm64-guest-owner"
if /i not "%OWNER%"=="%TASK%" exit /b 2
if not exist "%TASK%\" exit /b 2
if exist "%TASK%\identity-prompt.txt" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "PIF=%ROOT%\assets\release\PIF.EXE"
set "INI=%ROOT%\build\.ninja\fixtures-x64\guest-ini.exe"
"%PIF%" show "%DEST%\DOSPRMPT.PIF" >"%TASK%\identity-prompt.txt"
if errorlevel 1 exit /b 1
set "FLAGS="
for /f "usebackq tokens=1,2 delims==" %%A in ("%TASK%\identity-prompt.txt") do if "%%A"=="W386.FLAGS" set "FLAGS=%%B"
if not defined FLAGS exit /b 2
set /a "FLAGS=(FLAGS & ~8) | 2" >nul
if errorlevel 1 exit /b 2
call "%~dp0build-fixtures.cmd" x64 guest-ini
if errorlevel 1 exit /b 1
"%INI%" "%DEST%\SYSTEM.INI" "%TASK%\identity-no-insignia.ini" drop-file 386Enh device INSIGNIA.386
if errorlevel 1 exit /b 1
"%INI%" "%TASK%\identity-no-insignia.ini" "%TASK%\identity-no-bridge.ini" drop-file 386Enh device NTVDM64.386
if errorlevel 1 exit /b 1
call "%~dp0build-dos-identity.cmd" "%TASK%\fixtures" "%DEST%"
if errorlevel 1 exit /b 1
copy /b /y "%TASK%\identity-no-bridge.ini" "%DEST%\SYSTEM.INI" >nul || exit /b 1
(
 echo dos=high,umb
 echo device=%%SystemRoot%%\system32\himem.sys
 echo files=128
 if "%~3"=="1" echo dosonly
) >"%DEST%\PATCH\CONFIG.NT"
if errorlevel 1 exit /b 1
set "FAILED="
for %%I in (A B) do if not defined FAILED (
 copy /b /y "%TASK%\fixtures\%%I.COM" "%DEST%\%%I.COM" >nul
 if errorlevel 1 set "FAILED=1"
 copy /b /y "%DEST%\DOSPRMPT.PIF" "%DEST%\ID%%I.PIF" >nul
 if errorlevel 1 set "FAILED=1"
 if not defined FAILED "%PIF%" update "%DEST%\ID%%I.PIF" --title "Identity %%I" --directory "%DEST%" --arguments "" --w386-flags %FLAGS%
 if errorlevel 1 set "FAILED=1"
)
if defined FAILED exit /b 1
"%PIF%" update "%DEST%\PATCH\WIN.PIF" --arguments /3
exit /b %errorlevel%
