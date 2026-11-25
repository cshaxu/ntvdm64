@echo off
rem Owned install, evidence owner, package/input image, prompts|controller|load|empty-profile, options.
setlocal EnableExtensions DisableDelayedExpansion
if not "%~4"=="prompts" if not "%~4"=="controller" if not "%~4"=="load" if not "%~4"=="empty-profile" exit /b 2
if "%~4"=="load" if not "%~5"=="pair" if not "%~5"=="none" if not "%~5"=="only" exit /b 2
set "DEST=%~f1"
set "TASK=%~f2"
for %%I in ("%DEST%\..") do if /i not "%%~fI"=="O:\tmp" exit /b 2
if not exist "%DEST%\.ntvdm64-guest-owner" exit /b 2
set "OWNER="
set /p "OWNER="<"%DEST%\.ntvdm64-guest-owner"
if /i not "%OWNER%"=="%TASK%" exit /b 2
if not exist "%TASK%\" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
if "%~4"=="empty-profile" goto empty-profile
if "%~4"=="controller" call "%~dp0prepare-test-inputs.cmd" -GuestPatch "%DEST%" "%TASK%" "%~f3" VMTEST.EXE
if errorlevel 1 exit /b 1
if "%~4"=="prompts" if not "%~5"=="none" if not "%~5"=="empty" if not "%~5"=="seed" exit /b 2
if "%~4"=="prompts" call "%~dp0prepare-test-inputs.cmd" -GuestPatch "%DEST%" "%TASK%" "%~f3\system32\COMMAND.COM" COMMAND.COM
if errorlevel 1 exit /b 1
set "FAILED="
if "%~4"=="prompts" for %%I in (A B) do if not defined FAILED (
 "%ROOT%\assets\release\PIF.EXE" create "%DEST%\PATCH\%%I.PIF" --title "Prompt probe %%I" --program "%DEST%\PATCH\COMMAND.COM" --directory "%DEST%\PATCH" --arguments "" --config "" --autoexec "" --close-on-exit --w386-flags 0x21002 --min-memory 128 --max-memory 640
 if errorlevel 1 set "FAILED=1"
 if "%~5"=="empty" type nul >"%DEST%\PATCH\%%I.TXT"
 if errorlevel 1 set "FAILED=1"
 rem SET /P emits the exact witness bytes with no newline.
 if "%~5"=="seed" <nul >"%DEST%\PATCH\%%I.TXT" set /p "=seed%%I"
 if "%~5"=="seed" if not exist "%DEST%\PATCH\%%I.TXT" set "FAILED=1"
)
if defined FAILED exit /b 1
if "%~4"=="prompts" if "%~6"=="defer-load" exit /b 0
if "%~4"=="prompts" if not "%~6"=="" exit /b 2
call "%~dp0build-fixtures.cmd" x64 guest-ini
if errorlevel 1 exit /b 1
set "KEY=run"
set "VALUE=%DEST%\PATCH\VMTEST.EXE"
if not "%~4"=="controller" set "KEY=load"
if not "%~4"=="controller" set "VALUE=%DEST%\PATCH\A.PIF %DEST%\PATCH\B.PIF"
if "%~4"=="load" if "%~5"=="none" set "VALUE="
if "%~4"=="load" if "%~5"=="only" set "VALUE=%DEST%\PATCH\ONLY.PIF"
"%ROOT%\build\.ninja\fixtures-x64\guest-ini.exe" "%DEST%\WIN.INI" "%TASK%\editor-%KEY%.candidate" set windows "%KEY%" "%VALUE%"
if errorlevel 1 exit /b 1
copy /b /y "%TASK%\editor-%KEY%.candidate" "%DEST%\WIN.INI" >nul
exit /b %errorlevel%

:empty-profile
if not "%~3"=="profile" if not "%~3"=="dde" exit /b 2
if not "%~5"=="" exit /b 2
if exist "%DEST%\PROGMAN.INI\" exit /b 2
call "%~dp0build-fixtures.cmd" x86 win31-matrix
if errorlevel 1 exit /b 1
call "%~dp0prepare-test-inputs.cmd" "%TASK%" "%ROOT%\build\.ninja\fixtures-x86\guest-probes\win31-matrix\%~3.exe" "%~3.exe"
if errorlevel 1 exit /b 1
type nul >"%DEST%\PROGMAN.INI"
exit /b %errorlevel%
