@echo off
rem Owned install, evidence owner, 31|95, DOSONLY, multitask, install|select, optional Win31 mode 1|2 and selection label
setlocal EnableExtensions DisableDelayedExpansion
if not "%~9"=="" exit /b 2
if not "%~8"=="" if not "%~3"=="31" exit /b 2
if not "%~8"=="" if not "%~6"=="select" exit /b 2
for /f "delims=abcdefghijklmnopqrstuvwxyz0123456789-" %%L in ("%~8") do exit /b 2
if not "%~7"=="" if not "%~3"=="31" exit /b 2
if not "%~7"=="" if not "%~7"=="1" if not "%~7"=="2" exit /b 2
if "%~7"=="1" if not "%~5"=="0" exit /b 2
set "MODE=2"
if "%~7"=="1" set "MODE=1"
if not "%~6"=="install" if not "%~6"=="select" exit /b 2
if not "%~3"=="31" if not "%~3"=="95" exit /b 2
for %%V in ("%~4" "%~5") do if not "%%~V"=="0" if not "%%~V"=="1" exit /b 2
set "DEST=%~f1"
set "TASK=%~f2"
for %%I in ("%DEST%\..") do if /i not "%%~fI"=="O:\tmp" exit /b 2
if not exist "%DEST%\.ntvdm64-guest-owner" exit /b 2
set "OWNER="
set /p "OWNER="<"%DEST%\.ntvdm64-guest-owner"
if /i not "%OWNER%"=="%TASK%" exit /b 2
if not exist "%TASK%\" exit /b 2
set "SELECTOR=%TASK%\selector"
set "INPUT=%TASK%\select-input.txt"
if not "%~8"=="" set "SELECTOR=%TASK%\selector-%~8"
if not "%~8"=="" set "INPUT=%TASK%\select-%~8.txt"
if exist "%SELECTOR%" exit /b 2
if exist "%INPUT%" exit /b 2
set "LAUNCHER=win31-setup"
if "%~3"=="95" set "LAUNCHER=win95-launch"
if "%~6"=="install" call "%~dp0prepare-test-inputs.cmd" -GuestPatch "%DEST%" "%TASK%" "%~dp0%LAUNCHER%\WIN.CMD" WIN.CMD "%~dp0..\assets\release\PIF.EXE" PIF.EXE
if errorlevel 1 exit /b 1
if "%~6"=="install" call "%DEST%\PATCH\WIN.CMD" --install >"%TASK%\install.log" 2>&1
if errorlevel 1 exit /b 1
mkdir "%SELECTOR%" || exit /b 1
(
 echo @echo off
 echo exit /b 0
) >"%SELECTOR%\run16.cmd"
if errorlevel 1 exit /b 1
(
 if "%~3"=="31" echo %MODE%
 echo %~4
 if not "%MODE%"=="1" echo %~5
) >"%INPUT%"
if errorlevel 1 exit /b 1
set "PATH=%SELECTOR%;%PATH%"
call "%DEST%\PATCH\WIN.CMD" <"%INPUT%"
exit /b %errorlevel%
