@echo off
setlocal EnableExtensions DisableDelayedExpansion
if not "%~6"=="" exit /b 2
if not "%~1"=="ems" if not "%~1"=="xms" goto usage
if "%~2"=="" goto usage
if "%~3"=="" goto usage
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "OUTPUT=%~f2"
set "RUNTIME=%~3"
set "PROFILE=%~4"
set "STARTUP=%~5"
if "%~1"=="ems" (
  if not "%~4"=="" exit /b 2
  if not "%~5"=="" exit /b 2
  if not exist "%RUNTIME%\" exit /b 2
)
if not defined PROFILE set "PROFILE=default"
if not defined STARTUP set "STARTUP=normal"
if not "%PROFILE%"=="default" if not "%PROFILE%"=="default-low" if not "%PROFILE%"=="reserved" exit /b 2
if not "%STARTUP%"=="normal" if not "%STARTUP%"=="fault" exit /b 2
if exist "%OUTPUT%" exit /b 2
set "PARENT=%OUTPUT%"
:check_parent
set "PREVIOUS=%PARENT%"
for %%I in ("%PARENT%\..") do set "PARENT=%%~fI"
if /i "%PARENT%"=="%ROOT%\build" goto build
if /i "%PARENT%"=="%PREVIOUS%" exit /b 2
goto check_parent
:build
call "%~dp0build-fixtures.cmd" x86 memory-profiles
if errorlevel 1 exit /b %errorlevel%
set "CACHE=%ROOT%\build\.ninja\fixtures-x86"
set "PROBES=%CACHE%\guest-probes"
mkdir "%OUTPUT%"
if errorlevel 1 exit /b %errorlevel%
if "%~1"=="ems" goto ems
for %%F in (X35.COM R35.BAT) do (
  copy /b /y "%PROBES%\xms-common\%%F" "%OUTPUT%\%%F" >nul
  if errorlevel 1 exit /b 1
)
set "SOURCE=default"
set "OPTION=--default-int15"
if "%PROFILE%"=="default-low" set "OPTION=--default-low"
if "%PROFILE%"=="reserved" set "SOURCE=reserved"
if "%PROFILE%"=="reserved" set "OPTION="
copy /b /y "%PROBES%\xms-%SOURCE%\U35.COM" "%OUTPUT%\U35.COM" >nul
if errorlevel 1 exit /b %errorlevel%
copy /b /y "%PROBES%\xms-%STARTUP%\X35UMB.SYS" "%OUTPUT%\X35UMB.SYS" >nul
if errorlevel 1 exit /b %errorlevel%
"%CACHE%\xms_profile_builder.exe" "%OUTPUT%" "%RUNTIME%" %OPTION%
exit /b %errorlevel%
:ems
for %%F in (E30.COM D30.COM P30.COM) do (
  copy /b /y "%PROBES%\ems\%%F" "%OUTPUT%\%%F" >nul
  if errorlevel 1 exit /b 1
)
call "%~dp0prepare-memory-pif.cmd" "%RUNTIME%"
if errorlevel 1 exit /b %errorlevel%
"%CACHE%\pure_dos_pif_builder.exe" "%RUNTIME%"
exit /b %errorlevel%
:usage
echo Usage: build-memory-probes.cmd ems output runtime 1>&2
echo        build-memory-probes.cmd xms output runtime [default^|default-low^|reserved] [normal^|fault] 1>&2
exit /b 2
