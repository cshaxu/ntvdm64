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
call "%ROOT%\tools\build-fixtures.cmd" x64 guest-ini
if errorlevel 1 exit /b 1
set "INI=%ROOT%\build\.ninja\fixtures-x64\guest-ini.exe"
set "LOAD=%DEST%\PATCH\A.PIF %DEST%\PATCH\B.PIF"
if "%~6"=="1" set "LOAD=%DEST%\PATCH\B.PIF"
"%INI%" "%DEST%\WIN.INI" "%TASK%\editor-win.candidate" set windows load "%LOAD%"
if errorlevel 1 exit /b 1
if "%~7"=="1" "%INI%" "%DEST%\SYSTEM.INI" "%TASK%\editor-system.candidate" append 386Enh device *vmd
if errorlevel 1 exit /b 1
if not "%~4"=="-" (
 >"%TASK%\editor-autoexec.append" echo(
 >>"%TASK%\editor-autoexec.append" echo %DEST%\PATCH\MCALL.COM
)
if not "%~4"=="-" copy /b "%DEST%\PATCH\AUTOEXEC.NT"+"%TASK%\editor-autoexec.append" "%TASK%\editor-autoexec.candidate" >nul
if errorlevel 1 exit /b 1
copy /b /y "%PACKAGE%\system32\COMMAND.COM" "%DEST%\PATCH\COMMAND.COM" >nul
if errorlevel 1 exit /b 1
set "FAILED="
for %%I in (A B) do if not defined FAILED (
 "%ROOT%\assets\release\PIF.EXE" create "%DEST%\PATCH\%%I.PIF" --title "Prompt probe %%I" --program "%DEST%\PATCH\COMMAND.COM" --directory "%DEST%\PATCH" --arguments "" --config "" --autoexec "" --close-on-exit --w386-flags 0x21002 --min-memory 128 --max-memory 640
 if errorlevel 1 set "FAILED=1"
)
if defined FAILED exit /b 1
if "%~5"=="1" type nul >"%DEST%\PATCH\A.TXT"
if errorlevel 1 exit /b 1
if "%~5"=="1" type nul >"%DEST%\PATCH\B.TXT"
if errorlevel 1 exit /b 1
if not "%~4"=="-" copy /b /y "%~4" "%DEST%\PATCH\MCALL.COM" >nul
if errorlevel 1 exit /b 1
if not "%~4"=="-" copy /b /y "%TASK%\editor-autoexec.candidate" "%DEST%\PATCH\AUTOEXEC.NT" >nul
if errorlevel 1 exit /b 1
if "%~7"=="1" copy /b /y "%TASK%\editor-system.candidate" "%DEST%\SYSTEM.INI" >nul
if errorlevel 1 exit /b 1
copy /b /y "%TASK%\editor-win.candidate" "%DEST%\WIN.INI" >nul
if errorlevel 1 exit /b 1
call "%ROOT%\tools\prepare-win31-matrix.cmd" select "%DEST%" "%TASK%" %8 0
if errorlevel 1 exit /b 1
rem Use the current single-launcher contract; no legacy WINSTD/WIN386 files.
"%ROOT%\assets\release\PIF.EXE" create "%DEST%\PATCH\WIN.PIF" --title "Windows 3.1 386 Enhanced" --program "%DEST%\WIN.COM" --directory "%DEST%" --arguments /3 --config "%DEST%\PATCH\CONFIG.NT" --autoexec "%DEST%\PATCH\AUTOEXEC.NT" --close-on-exit --w386-max-xms 65535 --w386-flags 0x1003 --nt31-flags 1
if errorlevel 1 exit /b 1
exit /b 0
