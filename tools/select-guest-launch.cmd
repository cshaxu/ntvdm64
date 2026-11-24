@echo off
rem Owned install, evidence owner, 31|95, DOSONLY, multitask, install|select
setlocal EnableExtensions DisableDelayedExpansion
if not "%~7"=="" exit /b 2
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
if exist "%TASK%\selector" exit /b 2
if exist "%TASK%\select-input.txt" exit /b 2
set "LAUNCHER=win31-setup"
if "%~3"=="95" set "LAUNCHER=win95-launch"
if "%~6"=="install" call "%~dp0prepare-test-inputs.cmd" -GuestPatch "%DEST%" "%TASK%" "%~dp0%LAUNCHER%\WIN.CMD" WIN.CMD "%~dp0..\assets\release\PIF.EXE" PIF.EXE
if errorlevel 1 exit /b 1
if "%~6"=="install" call "%DEST%\PATCH\WIN.CMD" --install >"%TASK%\install.log" 2>&1
if errorlevel 1 exit /b 1
mkdir "%TASK%\selector" || exit /b 1
(
 echo @echo off
 echo exit /b 0
) >"%TASK%\selector\run16.cmd"
if errorlevel 1 exit /b 1
(
 if "%~3"=="31" echo 2
 echo %~4
 echo %~5
) >"%TASK%\select-input.txt"
if errorlevel 1 exit /b 1
set "PATH=%TASK%\selector;%PATH%"
call "%DEST%\PATCH\WIN.CMD" <"%TASK%\select-input.txt"
exit /b %errorlevel%
