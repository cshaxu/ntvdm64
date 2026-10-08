@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "PATCH=%~dp0"
set "PATCH=%PATCH:~0,-1%"
for %%I in ("%PATCH%\..") do set "MEDIA=%%~fI"
set "RESULT=1"

for %%F in (PIF.EXE HASH.EXE SETVER.EXE) do if not exist "%PATCH%\%%F" goto missing_tool
if not defined RUN16 (
  where /q run16
  if errorlevel 1 (
    echo Cannot find run16 in PATH.
    set "RESULT=2"
    goto cleanup
  )
  set "RUN16=run16"
)
for %%F in (SETUP.EXE SETUP.LBL KERNEL.EXE USER.EXE GDI.EXE DISK1 DISK2 DISK3 DISK4 DISK5 MOUSE.DRV MOUSE.DRV.BAK) do (
  if not exist "%MEDIA%\%%F" (
    echo Missing prepared media file: %%F
    set "RESULT=2"
    goto cleanup
  )
)
>"%PATCH%\CONFIG.NT" (
  echo REM Windows 1.01 dedicated DOS profile
  echo dos=high, umb
  echo device=%%SystemRoot%%\system32\himem.sys
  echo files=128
  echo dosonly
  echo device=%PATCH%\SETVER.EXE
) || goto work_failure
>"%PATCH%\AUTOEXEC.NT" (
  echo @echo off
  echo SET TEMP=%%SystemRoot%%\Temp
  echo SET TMP=%%SystemRoot%%\Temp
  echo SET PATH=%MEDIA%;%%SystemRoot%%\system32
) || goto work_failure
"%PATCH%\PIF.EXE" create "%PATCH%\SETUP.PIF" --title "Windows 1.01 Setup" --program "%MEDIA%\SETUP.EXE" --directory "%MEDIA%" --arguments "" --config "%PATCH%\CONFIG.NT" --autoexec "%PATCH%\AUTOEXEC.NT" --close-on-exit || goto work_failure
pushd "%MEDIA%" || goto work_failure
call "%RUN16%" "%PATCH%\SETUP.PIF"
set "RESULT=%errorlevel%"
popd
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
copy /y "%PATCH%\SETVER.EXE" "%ROOT%\PATCH\SETVER.EXE" >nul || exit /b 1
>"%ROOT%\PATCH\CONFIG.NT" (
  echo REM Windows 1.01 dedicated DOS profile
  echo dos=high, umb
  echo device=%%SystemRoot%%\system32\himem.sys
  echo files=128
  echo dosonly
  echo device=%ROOT%\PATCH\SETVER.EXE
) || exit /b 1
>"%ROOT%\PATCH\AUTOEXEC.NT" (
  echo @echo off
  echo SET TEMP=%%SystemRoot%%\Temp
  echo SET TMP=%%SystemRoot%%\Temp
  echo SET PATH=%ROOT%;%%SystemRoot%%\system32
) || exit /b 1
"%PATCH%\PIF.EXE" create "%ROOT%\PATCH\WIN101.PIF" --title "Windows 1.01" --program "%ROOT%\WIN.COM" --directory "%ROOT%" --arguments "" --config "%ROOT%\PATCH\CONFIG.NT" --autoexec "%ROOT%\PATCH\AUTOEXEC.NT" --close-on-exit || exit /b 1
copy /y "%ROOT%\PATCH\CONFIG.NT" "%ROOT%\CONFIG.NT" >nul || exit /b 1
copy /y "%ROOT%\PATCH\AUTOEXEC.NT" "%ROOT%\AUTOEXEC.NT" >nul || exit /b 1
copy /y "%ROOT%\PATCH\WIN101.PIF" "%ROOT%\WIN.PIF" >nul || exit /b 1
if exist "%ROOT%\SETUP.EXE" "%PATCH%\PIF.EXE" create "%ROOT%\SETUP.PIF" --title "Windows 1.01 Setup" --program "%ROOT%\SETUP.EXE" --directory "%ROOT%" --arguments "" --config "%ROOT%\PATCH\CONFIG.NT" --autoexec "%ROOT%\PATCH\AUTOEXEC.NT" || exit /b 1
>"%ROOT%\PATCH\WIN.CMD" (
  echo @echo off
  echo setlocal EnableExtensions DisableDelayedExpansion
  echo where /q run16
  echo if errorlevel 1 ^(echo Cannot find run16 in PATH.^& exit /b 2^)
  echo set "TEMP=%%SystemRoot%%\Temp"
  echo set "TMP=%%SystemRoot%%\Temp"
  echo pushd "%%~dp0.." ^|^| exit /b 3
  echo call run16 "%%~dp0WIN101.PIF"
  echo set "WIN101_RESULT=%%errorlevel%%"
  echo popd
  echo exit /b %%WIN101_RESULT%%
)
echo Installed PATCH profile is ready: %ROOT%\PATCH\WIN.CMD
exit /b 0

:missing_tool
echo PATCH is incomplete: PIF.EXE, HASH.EXE and SETVER.EXE are required.
set "RESULT=2"
goto cleanup

:work_failure
echo Could not prepare the Setup launch profile.
set "RESULT=1"

:cleanup
del /q "%PATCH%\SETUP.PIF" "%PATCH%\CONFIG.NT" "%PATCH%\AUTOEXEC.NT" 2>nul
echo Setup exit code: %RESULT%
pause
exit /b %RESULT%
