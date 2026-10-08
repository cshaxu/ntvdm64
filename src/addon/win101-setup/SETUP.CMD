@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "PATCH=%~dp0"
set "PATCH=%PATCH:~0,-1%"
for %%I in ("%PATCH%\..") do set "MEDIA=%%~fI"
set "WORK=%PATCH%\TEMP"
set "RESULT=1"
set "MADE_WORK="

if not exist "%PATCH%\PIF.EXE" goto missing_tool
if not exist "%PATCH%\HASH.EXE" goto missing_tool
if not exist "%PATCH%\MOUSE.DRV" goto missing_tool
if not exist "%PATCH%\SETVER.EXE" goto missing_tool
if not defined RUN16 (
  where /q run16
  if errorlevel 1 (
    echo Cannot find run16 in PATH.
    set "RESULT=2"
    goto cleanup
  )
  set "RUN16=run16"
)
for %%F in (SETUP.EXE SETUP.LBL KERNEL.EXE USER.EXE GDI.EXE DISK1 DISK2 DISK3 DISK4 DISK5) do (
  if not exist "%MEDIA%\%%F" (
    echo Missing original media file: %%F
    set "RESULT=2"
    goto cleanup
  )
)
if exist "%WORK%\" (
  echo PATCH\TEMP already exists. Preserve or remove it before retrying.
  set "RESULT=4"
  goto cleanup
)
mkdir "%WORK%" || goto work_failure
set "MADE_WORK=1"
"%PATCH%\HASH.EXE" "%PATCH%\MOUSE.DRV" > "%WORK%\MOUSE.SHA" || (
  set "RESULT=3"
  goto cleanup
)
set /p "MOUSE_HASH=" < "%WORK%\MOUSE.SHA"
del /q "%WORK%\MOUSE.SHA" >nul 2>nul
if /I not "%MOUSE_HASH%"=="DC01039B0FC2B7E9244F31A42A4AAA9ED10613291557B4B6E2695ECE1BA5666E" (
  echo MOUSE.DRV identity check failed.
  set "RESULT=3"
  goto cleanup
)
for %%F in ("%MEDIA%\*") do (
  if not exist "%%~fF\" if /I not "%%~nxF"=="PATCH" copy /y "%%~fF" "%WORK%\" >nul || goto work_failure
)
copy /y "%PATCH%\MOUSE.DRV" "%WORK%\MOUSE.DRV" >nul || goto work_failure
copy /y "%PATCH%\SETVER.EXE" "%WORK%\SETVER.EXE" >nul || goto work_failure
call :write_config "%WORK%\CONFIG.NT" "%WORK%\SETVER.EXE" || goto work_failure
call :write_autoexec "%WORK%\AUTOEXEC.NT" "%WORK%\TEMP" "%WORK%" || goto work_failure
mkdir "%WORK%\TEMP" || goto work_failure
"%PATCH%\PIF.EXE" create "%WORK%\SETUP.PIF" --title "Windows 1.01 Setup" --program "%WORK%\SETUP.EXE" --directory "%WORK%" --arguments "" --config "%WORK%\CONFIG.NT" --autoexec "%WORK%\AUTOEXEC.NT" || goto work_failure
pushd "%WORK%" || goto work_failure
call "%RUN16%" "%WORK%\SETUP.PIF"
set "RESULT=%errorlevel%"
popd
>"%PATCH%\setup-result.txt" echo run16_exit=%RESULT%
echo Original Setup returned %RESULT%.
set "INSTALL_ROOT="
set /p "INSTALL_ROOT=If installation completed, enter its actual directory; otherwise press Enter to finish: "
if not defined INSTALL_ROOT goto cleanup
call :configure_installed "%INSTALL_ROOT%" || set "RESULT=1"
goto cleanup

:configure_installed
set "ROOT=%~f1"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"
for %%F in (WIN.COM WIN100.BIN WIN100.OVL) do if not exist "%ROOT%\%%F" (
  echo Setup has not produced %ROOT%\%%F
  exit /b 1
)
if not exist "%ROOT%\PATCH\" mkdir "%ROOT%\PATCH" || exit /b 1
copy /y "%PATCH%\MOUSE.DRV" "%ROOT%\PATCH\MOUSE.DRV" >nul || exit /b 1
copy /y "%PATCH%\SETVER.EXE" "%ROOT%\PATCH\SETVER.EXE" >nul || exit /b 1
call :write_config "%ROOT%\PATCH\CONFIG.NT" "%ROOT%\PATCH\SETVER.EXE" || exit /b 1
call :write_autoexec "%ROOT%\PATCH\AUTOEXEC.NT" "%ROOT%\PATCH\TEMP" "%ROOT%" || exit /b 1
"%PATCH%\PIF.EXE" create "%ROOT%\PATCH\WIN101.PIF" --title "Windows 1.01" --program "%ROOT%\WIN.COM" --directory "%ROOT%" --arguments "" --config "%ROOT%\PATCH\CONFIG.NT" --autoexec "%ROOT%\PATCH\AUTOEXEC.NT" || exit /b 1
copy /y "%ROOT%\PATCH\CONFIG.NT" "%ROOT%\CONFIG.NT" >nul || exit /b 1
copy /y "%ROOT%\PATCH\AUTOEXEC.NT" "%ROOT%\AUTOEXEC.NT" >nul || exit /b 1
copy /y "%ROOT%\PATCH\WIN101.PIF" "%ROOT%\WIN.PIF" >nul || exit /b 1
if exist "%ROOT%\SETUP.EXE" "%PATCH%\PIF.EXE" create "%ROOT%\SETUP.PIF" --title "Windows 1.01 Setup" --program "%ROOT%\SETUP.EXE" --directory "%ROOT%" --arguments "" --config "%ROOT%\PATCH\CONFIG.NT" --autoexec "%ROOT%\PATCH\AUTOEXEC.NT" || exit /b 1
>"%ROOT%\PATCH\WIN.CMD" (
  echo @echo off
  echo setlocal EnableExtensions DisableDelayedExpansion
  echo where /q run16
  echo if errorlevel 1 ^(echo Cannot find run16 in PATH.^& exit /b 2^)
  echo set "TEMP=%%~dp0TEMP"
  echo set "TMP=%%TEMP%%"
  echo if not exist "%%TEMP%%\" mkdir "%%TEMP%%" ^|^| exit /b 3
  echo pushd "%%~dp0.." ^|^| exit /b 3
  echo call run16 "%%~dp0WIN101.PIF"
  echo set "WIN101_RESULT=%%errorlevel%%"
  echo popd
  echo rmdir /s /q "%%TEMP%%" 2^>nul
  echo exit /b %%WIN101_RESULT%%
)
echo Installed PATCH profile is ready: %ROOT%\PATCH\WIN.CMD
exit /b 0

:write_config
>"%~1" (
  echo REM Windows 1.01 dedicated DOS profile
  echo dos=high, umb
  echo device=%%SystemRoot%%\system32\himem.sys
  echo files=128
  echo dosonly
  echo device=%~2
)
exit /b 0

:write_autoexec
>"%~1" (
  echo @echo off
  echo SET TEMP=%~2
  echo SET TMP=%~2
  echo SET PATH=%~3;%%SystemRoot%%\system32
)
exit /b 0

:missing_tool
echo PATCH is incomplete: PIF.EXE, HASH.EXE, MOUSE.DRV and SETVER.EXE are required.
set "RESULT=2"
goto cleanup

:work_failure
echo Could not prepare private Setup media.
set "RESULT=1"

:cleanup
if defined MADE_WORK rmdir /s /q "%WORK%" 2>nul
echo Setup exit code: %RESULT%
pause
exit /b %RESULT%
