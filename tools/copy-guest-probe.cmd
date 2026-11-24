@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~3"=="" exit /b 2
if not "%~4"=="" exit /b 2
set "source=%~f1"
set "target=%~f2"
if not exist "%source%\WIN.COM" exit /b 2
if not exist "%source%\SYSTEM.INI" exit /b 2
call "%~dp0create-guest-probe.cmd" "%target%" "%~f3"
if errorlevel 1 exit /b %errorlevel%
rem A copied source's marker never grants ownership of this new directory.
robocopy "%source%" "%target%" /E /COPY:DAT /DCOPY:DAT /R:0 /W:0 /XF .ntvdm64-guest-owner .t479-owner
if errorlevel 8 exit /b 1
exit /b 0
