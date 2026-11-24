@echo off
rem Owned copy, owner, package, optional MCALL.COM or -, save, staged, VMD, DOSONLY.
setlocal EnableExtensions DisableDelayedExpansion
if not "%~9"=="" exit /b 2
for %%V in ("%~5" "%~6" "%~7" "%~8") do if not "%%~V"=="0" if not "%%~V"=="1" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "DEST=%~f1"
set "TASK=%~f2"
set "PACKAGE=%~f3"
for %%I in ("%DEST%\..") do if /i not "%%~fI"=="O:\tmp" exit /b 2
if not exist "%DEST%\.ntvdm64-guest-owner" exit /b 2
set "OWNER="
set /p "OWNER="<"%DEST%\.ntvdm64-guest-owner"
if /i not "%OWNER%"=="%TASK%" exit /b 2
if not exist "%TASK%\" exit /b 2
if not exist "%PACKAGE%\system32\COMMAND.COM" exit /b 2
if not "%~4"=="-" if not exist "%~4" exit /b 2
if not exist "%DEST%\PATCH\AUTOEXEC.NT" exit /b 2
if exist "%TASK%\editor-autoexec.append" exit /b 2
if exist "%TASK%\editor-autoexec.candidate" exit /b 2
if not "%~4"=="-" (
 >"%TASK%\editor-autoexec.append" echo(
 >>"%TASK%\editor-autoexec.append" echo %DEST%\PATCH\MCALL.COM
)
if not "%~4"=="-" copy /b "%DEST%\PATCH\AUTOEXEC.NT"+"%TASK%\editor-autoexec.append" "%TASK%\editor-autoexec.candidate" >nul
if errorlevel 1 exit /b 1
set "WITNESS=none"
if "%~5"=="1" set "WITNESS=empty"
call "%~dp0prepare-guest-witness.cmd" "%DEST%" "%TASK%" "%PACKAGE%" prompts %WITNESS% defer-load
if errorlevel 1 exit /b 1
if not "%~4"=="-" copy /b /y "%~4" "%DEST%\PATCH\MCALL.COM" >nul
if errorlevel 1 exit /b 1
if not "%~4"=="-" copy /b /y "%TASK%\editor-autoexec.candidate" "%DEST%\PATCH\AUTOEXEC.NT" >nul
if errorlevel 1 exit /b 1
set "LOAD_CASE=pair"
if "%~6"=="1" set "LOAD_CASE=second"
call "%ROOT%\tools\prepare-win31-matrix.cmd" launch "%DEST%" "%TASK%" %8 0 %LOAD_CASE% %7
exit /b %errorlevel%
