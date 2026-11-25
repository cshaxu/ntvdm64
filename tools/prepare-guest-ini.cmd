@echo off
rem STAGE Empty|WfwNet, or STAGE Drivers OWNER 31|95 BRIDGE INSTANCE INTS RETIRE MOUSE.
rem Optional driver inputs use -; only owned copied installations may be changed.
setlocal EnableExtensions DisableDelayedExpansion
set "DRIVERS="
if "%~2"=="Drivers" set "DRIVERS=1"
if not defined DRIVERS if not "%~3"=="" exit /b 2
if not defined DRIVERS if not "%~2"=="Empty" if not "%~2"=="WfwNet" exit /b 2
if defined DRIVERS if "%~9"=="" exit /b 2
if defined DRIVERS if not "%~4"=="31" if not "%~4"=="95" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "STAGE=%~f1"
set "TASK=%~f3"
for %%I in ("%STAGE%\..") do if /i not "%%~fI"=="O:\tmp" exit /b 2
if not exist "%STAGE%\system.ini" exit /b 2
if not defined DRIVERS if not exist "%STAGE%\.ntvdm64-test-stage.json" exit /b 2
if defined DRIVERS if not exist "%STAGE%\.ntvdm64-guest-owner" exit /b 2
set "OWNER="
if defined DRIVERS set /p "OWNER="<"%STAGE%\.ntvdm64-guest-owner"
if defined DRIVERS if /i not "%OWNER%"=="%TASK%" exit /b 2
set "TOOL=%ROOT%\build\.ninja\addon\ini\ini.exe"
if not defined DRIVERS if not exist "%TOOL%" exit /b 2
set "HASH="
if "%~2"=="WfwNet" for /f "delims=" %%H in ('call "%ROOT%\assets\release\HASH.EXE" "%STAGE%\system\WFWNET.DRV"') do set "HASH=%%H"
if "%~2"=="WfwNet" if /i not "%HASH%"=="4C321D43511F845EC1A95518507133D1CF4CF3993BAE422037DBA95CE6258CCA" exit /b 2
if not defined DRIVERS "%TOOL%" "%STAGE%\system.ini" "%~2"
if not defined DRIVERS exit /b %errorlevel%
if not exist "%TASK%\" exit /b 2
if not exist "%STAGE%\PATCH\" exit /b 2
if "%~5"=="-" exit /b 2
set "INVALID="
for %%F in ("%~5" "%~6" "%~7" "%~8" "%~9") do if not "%%~F"=="-" if not exist "%%~F" set "INVALID=1"
if defined INVALID exit /b 2
set /a FLAGS=0 >nul
if not "%~6"=="-" set /a FLAGS+=1 >nul
if not "%~7"=="-" set /a FLAGS+=2 >nul
if not "%~8"=="-" set /a FLAGS+=4 >nul
if not "%~9"=="-" set /a FLAGS+=8 >nul
call "%ROOT%\tools\build-addon.cmd" ini
if errorlevel 1 exit /b 1
"%TOOL%" "%STAGE%\SYSTEM.INI" "%TASK%\driver-profile.candidate" "%STAGE%" "%~4" %FLAGS%
if errorlevel 1 exit /b 1
copy /b /y "%~5" "%STAGE%\PATCH\NTVDM64.386" >nul
if errorlevel 1 exit /b 1
if not "%~6"=="-" copy /b /y "%~6" "%STAGE%\PATCH\INSIGNIA.386" >nul
if errorlevel 1 exit /b 1
if not "%~7"=="-" copy /b /y "%~7" "%STAGE%\PATCH\INTS.386" >nul
if errorlevel 1 exit /b 1
if not "%~8"=="-" copy /b /y "%~8" "%STAGE%\PATCH\RETIRE.386" >nul
if errorlevel 1 exit /b 1
if not "%~9"=="-" copy /b /y "%~9" "%STAGE%\SYSTEM\MOUSE.DRV" >nul
if errorlevel 1 exit /b 1
copy /b /y "%TASK%\driver-profile.candidate" "%STAGE%\SYSTEM.INI" >nul
if errorlevel 1 exit /b 1
exit /b 0
