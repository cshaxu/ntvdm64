@echo off
rem prepare|case|select|restore INSTALL OUTPUT DOSONLY ALLOW_USER_INSTALLATION
setlocal EnableExtensions DisableDelayedExpansion
if not "%~1"=="prepare" if not "%~1"=="case" if not "%~1"=="select" if not "%~1"=="restore" exit /b 2
if not "%~4"=="0" if not "%~4"=="1" exit /b 2
if not "%~5"=="0" if not "%~5"=="1" exit /b 2
if not "%~6"=="" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "DEST=%~f2"
set "TASK=%~f3"
set "ALLOWED="
for %%I in ("%DEST%\..") do if /i "%%~fI"=="O:\tmp" set "ALLOWED=1"
if "%~5"=="1" for %%I in (O:\w31 O:\w31t) do if /i "%%I"=="%DEST%" set "ALLOWED=1"
if not defined ALLOWED exit /b 2
if not exist "%TASK%\" exit /b 2
if not exist "%DEST%\PATCH\CONFIG.NT" exit /b 2
if not exist "%DEST%\PATCH\AUTOEXEC.NT" exit /b 2
if not exist "%DEST%\WIN.INI" exit /b 2
set "FAILED="
if "%~1"=="prepare" (
 for %%F in (win.ini config.nt autoexec.nt) do if exist "%TASK%\matrix-original-%%F" set "FAILED=1"
 if exist "%TASK%\matrix-prepared" set "FAILED=1"
 if not exist "%TASK%\W31CHECK.EXE" set "FAILED=1"
 if not exist "%TASK%\W31DOS.COM" set "FAILED=1"
)
if defined FAILED exit /b 2
if "%~1"=="case" if not exist "%TASK%\matrix-prepared" exit /b 2
if "%~1"=="restore" if not exist "%TASK%\matrix-prepared" exit /b 2
set "SNAPSHOT_OWNER="
if exist "%TASK%\matrix-prepared" set /p "SNAPSHOT_OWNER="<"%TASK%\matrix-prepared"
if defined SNAPSHOT_OWNER if /i not "%SNAPSHOT_OWNER%"=="%DEST%" exit /b 2
if "%~1"=="prepare" call "%ROOT%\tools\build-fixtures.cmd" x64 guest-ini
if errorlevel 1 exit /b 1
if "%~1"=="prepare" "%ROOT%\build\.ninja\fixtures-x64\win31-probe-paths.exe" "%TASK%\W31CHECK.EXE" "%TASK%\W31DOS.COM" "%DEST%\PATCH"
if errorlevel 1 exit /b 1
if "%~1"=="prepare" "%ROOT%\build\.ninja\fixtures-x64\guest-ini.exe" "%DEST%\WIN.INI" "%TASK%\matrix-win.candidate" set windows run "%DEST%\PATCH\W31CHECK.EXE"
if errorlevel 1 exit /b 1
if "%~1"=="prepare" copy /b "%DEST%\WIN.INI" "%TASK%\matrix-original-win.ini" >nul
if errorlevel 1 exit /b 1
if "%~1"=="prepare" copy /b "%DEST%\PATCH\CONFIG.NT" "%TASK%\matrix-original-config.nt" >nul
if errorlevel 1 exit /b 1
if "%~1"=="prepare" copy /b "%DEST%\PATCH\AUTOEXEC.NT" "%TASK%\matrix-original-autoexec.nt" >nul
if errorlevel 1 exit /b 1
rem Snapshot completion precedes the first guest-copy mutation, permitting restore on failure.
if "%~1"=="prepare" >"%TASK%\matrix-prepared" echo %DEST%
if errorlevel 1 exit /b 1
if "%~1"=="prepare" (
 >"%TASK%\matrix-autoexec.append" echo(
 >>"%TASK%\matrix-autoexec.append" echo %DEST%\PATCH\W31DOS.COM
)
if "%~1"=="prepare" copy /b "%TASK%\matrix-original-autoexec.nt"+"%TASK%\matrix-autoexec.append" "%TASK%\matrix-autoexec.candidate" >nul
if errorlevel 1 exit /b 1
if "%~1"=="prepare" copy /b /y "%TASK%\matrix-autoexec.candidate" "%DEST%\PATCH\AUTOEXEC.NT" >nul
if errorlevel 1 exit /b 1
if "%~1"=="prepare" copy /b /y "%TASK%\matrix-win.candidate" "%DEST%\WIN.INI" >nul
if errorlevel 1 exit /b 1
if "%~1"=="prepare" for %%F in (W31CHECK.EXE W31DOS.COM) do if not defined FAILED (
 copy /b "%TASK%\%%F" "%DEST%\PATCH\%%F" >nul
 if errorlevel 1 set "FAILED=1"
)
if defined FAILED exit /b 1
set "CONFIG_SOURCE="
if "%~1"=="case" set "CONFIG_SOURCE=%TASK%\matrix-original-config.nt"
if "%~1"=="select" set "CONFIG_SOURCE=%DEST%\PATCH\CONFIG.NT"
if defined CONFIG_SOURCE (
 rem CONFIG.NT is DOS text. Filter only the standalone directive; preserve other lines.
 findstr /v /i /r /x /c:"[ 	]*dosonly[ 	]*" "%CONFIG_SOURCE%" >"%TASK%\matrix-config.candidate"
 if errorlevel 2 exit /b 1
)
if defined CONFIG_SOURCE if "%~4"=="1" (
 >>"%TASK%\matrix-config.candidate" echo(
 >>"%TASK%\matrix-config.candidate" echo dosonly
)
if defined CONFIG_SOURCE copy /b /y "%TASK%\matrix-config.candidate" "%DEST%\PATCH\CONFIG.NT" >nul
if errorlevel 1 exit /b 1
if "%~1"=="restore" copy /b /y "%TASK%\matrix-original-win.ini" "%DEST%\WIN.INI" >nul
if errorlevel 1 exit /b 1
if "%~1"=="restore" for %%F in (config.nt autoexec.nt) do if not defined FAILED (
 copy /b /y "%TASK%\matrix-original-%%F" "%DEST%\PATCH\%%F" >nul
 if errorlevel 1 set "FAILED=1"
)
if defined FAILED exit /b 1
exit /b 0
