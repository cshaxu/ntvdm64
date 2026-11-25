@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~1"=="" exit /b 2
if not "%~2"=="" exit /b 2
set "STAGE=%~f1"
for %%I in ("%STAGE%\..") do if /i not "%%~fI"=="O:\tmp" exit /b 2
if not exist "%STAGE%\.ntvdm64-test-stage.json" exit /b 2
if not exist "%STAGE%\tests\S6PROBE.COM" exit /b 2
set "BINARY=%STAGE%"
if exist "%STAGE%\system32\run16.exe" set "BINARY=%STAGE%\system32"
for %%I in (E30.COM E30.PIF E30.NT E30AUTO.NT) do if exist "%BINARY%\%%I" exit /b 2
copy /b "%STAGE%\tests\S6PROBE.COM" "%BINARY%\E30.COM" >nul
if errorlevel 1 exit /b 1
call "%~dp0prepare-memory-pif.cmd" "%BINARY%" --close-on-exit
exit /b %errorlevel%
