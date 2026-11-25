@echo off
rem prepare|case|select|restore INSTALL OUTPUT DOSONLY ALLOW_USER_INSTALLATION
rem launch INSTALL OWNER DOSONLY 0 pair|second|none ADD_VMD
setlocal EnableExtensions DisableDelayedExpansion
if "%~1"=="setup" goto setup
if not "%~1"=="prepare" if not "%~1"=="case" if not "%~1"=="select" if not "%~1"=="restore" if not "%~1"=="launch" exit /b 2
if not "%~4"=="0" if not "%~4"=="1" exit /b 2
if not "%~5"=="0" if not "%~5"=="1" exit /b 2
if not "%~1"=="launch" if not "%~6"=="" exit /b 2
if "%~1"=="launch" if not "%~5"=="0" exit /b 2
if "%~1"=="launch" if not "%~6"=="pair" if not "%~6"=="second" if not "%~6"=="none" exit /b 2
if "%~1"=="launch" if not "%~7"=="0" if not "%~7"=="1" exit /b 2
if not "%~8"=="" exit /b 2
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
set "OWNER="
if "%~1"=="launch" if not exist "%DEST%\.ntvdm64-guest-owner" exit /b 2
if "%~1"=="launch" set /p "OWNER="<"%DEST%\.ntvdm64-guest-owner"
if "%~1"=="launch" if /i not "%OWNER%"=="%TASK%" exit /b 2
if "%~1"=="launch" call "%ROOT%\tools\build-addon.cmd" ini
if errorlevel 1 exit /b 1
set "LOAD="
if "%~6"=="pair" set "LOAD=%DEST%\PATCH\A.PIF %DEST%\PATCH\B.PIF"
if "%~6"=="second" set "LOAD=%DEST%\PATCH\B.PIF"
set "INI=%ROOT%\build\.ninja\addon\ini\ini.exe"
if "%~1"=="launch" "%INI%" "%DEST%\WIN.INI" "%TASK%\probe-win.candidate" set windows load "%LOAD%"
if errorlevel 1 exit /b 1
if "%~1"=="launch" if "%~7"=="1" "%INI%" "%DEST%\SYSTEM.INI" "%TASK%\probe-system.candidate" append 386Enh device *vmd
if errorlevel 1 exit /b 1
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
if "%~1"=="prepare" call "%ROOT%\tools\build-addon.cmd" ini
if errorlevel 1 exit /b 1
if "%~1"=="prepare" "%ROOT%\build\.ninja\fixtures-x64\win31-probe-paths.exe" "%TASK%\W31CHECK.EXE" "%TASK%\W31DOS.COM" "%DEST%\PATCH"
if errorlevel 1 exit /b 1
if "%~1"=="prepare" "%ROOT%\build\.ninja\addon\ini\ini.exe" "%DEST%\WIN.INI" "%TASK%\matrix-win.candidate" set windows run "%DEST%\PATCH\W31CHECK.EXE"
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
if "%~1"=="launch" set "CONFIG_SOURCE=%DEST%\PATCH\CONFIG.NT"
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
if "%~1"=="launch" "%ROOT%\assets\release\PIF.EXE" create "%DEST%\PATCH\WIN.PIF" --title "Windows 3.1 386 Enhanced" --program "%DEST%\WIN.COM" --directory "%DEST%" --arguments /3 --config "%DEST%\PATCH\CONFIG.NT" --autoexec "%DEST%\PATCH\AUTOEXEC.NT" --close-on-exit --w386-max-xms 65535 --w386-flags 0x1003 --nt31-flags 1
if errorlevel 1 exit /b 1
if "%~1"=="launch" if "%~7"=="1" copy /b /y "%TASK%\probe-system.candidate" "%DEST%\SYSTEM.INI" >nul
if errorlevel 1 exit /b 1
if "%~1"=="launch" copy /b /y "%TASK%\probe-win.candidate" "%DEST%\WIN.INI" >nul
if errorlevel 1 exit /b 1
if "%~1"=="restore" copy /b /y "%TASK%\matrix-original-win.ini" "%DEST%\WIN.INI" >nul
if errorlevel 1 exit /b 1
if "%~1"=="restore" for %%F in (config.nt autoexec.nt) do if not defined FAILED (
 copy /b /y "%TASK%\matrix-original-%%F" "%DEST%\PATCH\%%F" >nul
 if errorlevel 1 set "FAILED=1"
)
if defined FAILED exit /b 1
exit /b 0

:setup
rem Reapply the real Setup finalization to a disposable installation.
if not "%~4"=="" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "DEST=%~f2"
set "TASK=%~f3"
for %%I in ("%DEST%\..") do if /i not "%%~fI"=="O:\tmp" exit /b 2
if not exist "%DEST%\.ntvdm64-guest-owner" exit /b 2
set "OWNER="
set /p "OWNER="<"%DEST%\.ntvdm64-guest-owner"
if /i not "%OWNER%"=="%TASK%" exit /b 2
if not exist "%TASK%\" exit /b 2
if exist "%TASK%\setup-shim" exit /b 2
call "%ROOT%\tools\build-addon.cmd" ini
if errorlevel 1 exit /b 1
set "INI=%ROOT%\build\.ninja\addon\ini\ini.exe"
set "PATCH=%DEST%\PATCH"
for %%D in (INSIGNIA.386 NTVDM64.386) do (
 fc /b "%PATCH%\%%D" "%ROOT%\assets\release\%%D" >nul
 if errorlevel 1 exit /b 1
)
"%INI%" "%DEST%\SYSTEM.INI" "%TASK%\setup-no-insignia.ini" drop-file 386Enh device INSIGNIA.386
if errorlevel 1 exit /b 1
"%INI%" "%TASK%\setup-no-insignia.ini" "%TASK%\setup-system.ini" drop-file 386Enh device NTVDM64.386
if errorlevel 1 exit /b 1
for %%F in (INSIGNIA.386 NTVDM64.386 CONFIG.NT AUTOEXEC.NT WINSTD.PIF WIN386.PIF WINSTD.CMD WIN386.CMD) do if exist "%PATCH%\%%F\" exit /b 2
for %%F in (INSIGNIA.386 NTVDM64.386 CONFIG.NT AUTOEXEC.NT WINSTD.PIF WIN386.PIF WINSTD.CMD WIN386.CMD) do if exist "%PATCH%\%%F" (
 del /q "%PATCH%\%%F"
 if errorlevel 1 exit /b 1
)
copy /b /y "%TASK%\setup-system.ini" "%DEST%\SYSTEM.INI" >nul || exit /b 1
set "MEDIA=%TASK%\media\PATCH"
call "%ROOT%\tools\prepare-test-inputs.cmd" "%MEDIA%" "%ROOT%\tools\win31-setup\SETUP.CMD" SETUP.CMD "%ROOT%\tools\win31-setup\WIN.CMD" WIN.CMD "%ROOT%\assets\release\PIF.EXE" PIF.EXE "%ROOT%\assets\release\INI.EXE" INI.EXE "%ROOT%\assets\release\INSIGNIA.386" INSIGNIA.386 "%ROOT%\assets\release\NTVDM64.386" NTVDM64.386
if errorlevel 1 exit /b 1
mkdir "%TASK%\setup-shim" || exit /b 1
(
 echo @echo off
 echo exit /b 0
) >"%TASK%\setup-shim\run16.cmd"
if errorlevel 1 exit /b 1
>"%TASK%\setup-input.txt" echo %DEST%
if errorlevel 1 exit /b 1
set "PATH=%TASK%\setup-shim;%PATH%"
set "NTVDM_SETUP_TEST_NO_PAUSE=1"
call "%MEDIA%\SETUP.CMD" <"%TASK%\setup-input.txt"
exit /b %errorlevel%
