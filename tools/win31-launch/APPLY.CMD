@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET="
set "RESULT=1"
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
echo Windows 3.1 NTVDM launch preparation
echo.
set /p "TARGET=Target installed Windows 3.1 directory: "
if not defined TARGET (set "RESULT=64"& goto done)
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
for %%F in (WIN.COM SYSTEM\MOUSE.DRV SYSTEM.INI SYSTEM\KRNL386.EXE SYSTEM\WIN386.EXE) do if not exist "%TARGET%\%%F" (
  echo Missing installed Windows file: %%F
  set "RESULT=2"
  goto done
)
if not exist "%~dp0PATCH386.EXE" (echo Missing PATCH386.EXE beside APPLY.CMD.& set "RESULT=3"& goto done)
for %%F in (PIF.EXE HASH.EXE MOUSE31.DRV) do if not exist "%RELEASE%\%%F" (
  echo Missing released add-on: %RELEASE%\%%F
  set "RESULT=3"
  goto done
)
set "PATCH=%TARGET%\PATCH"
if not exist "%PATCH%\" mkdir "%PATCH%" || (echo Could not create "%PATCH%".& set "RESULT=4"& goto done)
if not exist "%PATCH%\TEMP\" (
  echo New: TEMP =^> %PATCH%
  mkdir "%PATCH%\TEMP" || (echo Could not create "%PATCH%\TEMP".& set "RESULT=4"& goto done)
)
call :install_system_config || goto mutation_failure
call :backup_retail "%TARGET%\SYSTEM\KRNL386.EXE" FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980 75E931AE000FB7C70CF686DA6F7D3FCB032FDC0E0A4E3187ED68179B85E2CD4A || goto mutation_failure
call :backup_retail "%TARGET%\SYSTEM\WIN386.EXE" 6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5 C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8 || goto mutation_failure
"%~dp0PATCH386.EXE" --apply --krnl "%TARGET%\SYSTEM\KRNL386.EXE" --win386 "%TARGET%\SYSTEM\WIN386.EXE" || goto mutation_failure
call :install_mouse || goto mutation_failure
call :announce_profile "%PATCH%\CONFIG.NT"
>"%PATCH%\CONFIG.NT" (
  echo dos=high, umb
  echo device=%%SystemRoot%%\system32\himem.sys
  echo files=128
)
call :announce_profile "%PATCH%\AUTOEXEC.NT"
>"%PATCH%\AUTOEXEC.NT" (
  echo @echo off
  echo lh %%SystemRoot%%\system32\mscdexnt.exe
  echo lh %%SystemRoot%%\system32\redir
  echo SET TEMP=%PATCH%\TEMP
  echo SET TMP=%PATCH%\TEMP
  echo SET PATH=%TARGET%;%TARGET%\SYSTEM;%%SystemRoot%%\system32
)
call :announce_profile "%PATCH%\WINSTD.PIF"
"%RELEASE%\PIF.EXE" create "%PATCH%\WINSTD.PIF" --title "Windows 3.1 Standard" --program "%TARGET%\WIN.COM" --directory "%TARGET%" --arguments "/S" --config "%PATCH%\CONFIG.NT" --autoexec "%PATCH%\AUTOEXEC.NT" --close-on-exit --w386-max-xms 65535 --w386-flags 0x1003 --nt31-flags 1 || goto mutation_failure
call :announce_profile "%PATCH%\WIN386.PIF"
"%RELEASE%\PIF.EXE" create "%PATCH%\WIN386.PIF" --title "Windows 3.1 386 Enhanced" --program "%TARGET%\WIN.COM" --directory "%TARGET%" --arguments "/3" --config "%PATCH%\CONFIG.NT" --autoexec "%PATCH%\AUTOEXEC.NT" --close-on-exit --w386-max-xms 65535 --w386-flags 0x1003 --nt31-flags 1 || goto mutation_failure
call :announce_profile "%PATCH%\WINSTD.CMD"
>"%PATCH%\WINSTD.CMD" (
  echo @echo off
  echo setlocal
  echo where /q run16 ^|^| exit /b 2
  echo set "TEMP=%%~dp0TEMP"
  echo set "TMP=%%TEMP%%"
  echo if not exist "%%TEMP%%\" mkdir "%%TEMP%%" ^|^| exit /b 4
  echo pushd "%%~dp0.." ^|^| exit /b 3
  echo call run16 "%%~dp0WINSTD.PIF"
  echo set result=%%errorlevel%%
  echo popd
  echo exit /b %%result%%
)
call :announce_profile "%PATCH%\WIN386.CMD"
>"%PATCH%\WIN386.CMD" (
  echo @echo off
  echo setlocal
  echo where /q run16 ^|^| exit /b 2
  echo set "TEMP=%%~dp0TEMP"
  echo set "TMP=%%TEMP%%"
  echo if not exist "%%TEMP%%\" mkdir "%%TEMP%%" ^|^| exit /b 4
  echo pushd "%%~dp0.." ^|^| exit /b 3
  echo call run16 "%%~dp0WIN386.PIF"
  echo set result=%%errorlevel%%
  echo popd
  echo exit /b %%result%%
)
set "RESULT=0"
goto done

:backup_retail
set "CURRENT=" & set "BACKUP=" & set "RETAIL=%~2" & set "CANDIDATE=%~3"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%~1"') do set "CURRENT=%%H"
if not defined CURRENT exit /b 1
if /I "%CURRENT%"=="%RETAIL%" (
  if exist "%~1.BAK" (
    for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%~1.BAK"') do set "BACKUP=%%H"
    call :require_retail_backup "%~1.BAK" || exit /b 1
    exit /b 0
  )
  echo Replace: %~nx1 =^> %TARGET%\SYSTEM
  move /y "%~1" "%~1.BAK" >nul || exit /b 1
  copy /y "%~1.BAK" "%~1" >nul || exit /b 1
  exit /b 0
)
if /I not "%CURRENT%"=="%CANDIDATE%" (echo Unsupported installed binary: %~1& exit /b 1)
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%~1.BAK"') do set "BACKUP=%%H"
if /I not "%BACKUP%"=="%RETAIL%" (echo Patched binary lacks retail recovery copy: %~1.BAK& exit /b 1)
exit /b 0

:require_retail_backup
if /I not "%BACKUP%"=="%RETAIL%" (echo Recovery copy is not the original: %~1& exit /b 1)
exit /b 0

:install_mouse
set "CURRENT=" & set "RELEASE_HASH="
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%RELEASE%\MOUSE31.DRV"') do set "RELEASE_HASH=%%H"
if not defined RELEASE_HASH exit /b 1
if not exist "%TARGET%\SYSTEM\MOUSE.DRV.BAK" (
  if exist "%PATCH%\MOUSE.DRV.ORIG" (
    move /y "%PATCH%\MOUSE.DRV.ORIG" "%TARGET%\SYSTEM\MOUSE.DRV.BAK" >nul || exit /b 1
  ) else (
    move /y "%TARGET%\SYSTEM\MOUSE.DRV" "%TARGET%\SYSTEM\MOUSE.DRV.BAK" >nul || exit /b 1
  )
) else (
  call :verify_released_mouse || exit /b 1
)
echo Replace: MOUSE.DRV =^> %TARGET%\SYSTEM
copy /y "%RELEASE%\MOUSE31.DRV" "%TARGET%\SYSTEM\MOUSE.DRV" >nul || exit /b 1
exit /b 0

:verify_released_mouse
set "CURRENT=" & set "RELEASE_HASH="
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%RELEASE%\MOUSE31.DRV"') do set "RELEASE_HASH=%%H"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%TARGET%\SYSTEM\MOUSE.DRV"') do set "CURRENT=%%H"
if not defined CURRENT exit /b 1
if not defined RELEASE_HASH exit /b 1
if /I not "%CURRENT%"=="%RELEASE_HASH%" (
  echo Refusing to overwrite an unrecognised installed mouse driver.
  exit /b 1
)
exit /b 0

:install_system_config
set "CONFIG_BACKUP=%PATCH%\SYSTEM.INI.BAK"
set "CONFIG_HASH=%PATCH%\SYSTEM.INI.SHA256"
set "CONFIG_WORK=%PATCH%\SYSTEM.INI.TMP"
set "CONFIG_NEW="
if exist "%CONFIG_WORK%" (echo Existing SYSTEM.INI temporary file blocks preparation.& exit /b 1)
if exist "%CONFIG_BACKUP%" (
  if not exist "%CONFIG_HASH%" (echo SYSTEM.INI recovery record is incomplete.& exit /b 1)
  call :verify_system_config || exit /b 1
) else (
  if exist "%CONFIG_HASH%" (echo SYSTEM.INI recovery record is incomplete.& exit /b 1)
  copy /y "%TARGET%\SYSTEM.INI" "%CONFIG_BACKUP%" >nul || exit /b 1
  set "CONFIG_NEW=1"
)
copy /y "%TARGET%\SYSTEM.INI" "%CONFIG_WORK%" >nul || (if defined CONFIG_NEW del /q "%CONFIG_BACKUP%" 2>nul & exit /b 1)
"%~dp0PATCH386.EXE" --config "%TARGET%\SYSTEM.INI"
if errorlevel 1 (
  call :rollback_system_config
  exit /b 1
)
set "CONFIG_CURRENT="
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%TARGET%\SYSTEM.INI"') do set "CONFIG_CURRENT=%%H"
if not defined CONFIG_CURRENT (call :rollback_system_config & exit /b 1)
>"%CONFIG_HASH%" echo %CONFIG_CURRENT%
if errorlevel 1 (call :rollback_system_config & exit /b 1)
del /q "%CONFIG_WORK%" 2>nul
echo Configure: SYSTEM.INI =^> %TARGET%
exit /b 0

:rollback_system_config
copy /y "%CONFIG_WORK%" "%TARGET%\SYSTEM.INI" >nul || exit /b 1
del /q "%CONFIG_WORK%" 2>nul
if defined CONFIG_NEW del /q "%CONFIG_BACKUP%" "%CONFIG_HASH%" 2>nul
exit /b 0

:verify_system_config
set "CONFIG_CURRENT=" & set "CONFIG_EXPECTED="
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%TARGET%\SYSTEM.INI"') do set "CONFIG_CURRENT=%%H"
set /p "CONFIG_EXPECTED=" < "%CONFIG_HASH%"
if not defined CONFIG_CURRENT exit /b 1
if /I not "%CONFIG_CURRENT%"=="%CONFIG_EXPECTED%" (echo SYSTEM.INI changed since preparation.& exit /b 1)
exit /b 0

:announce_profile
if exist "%~1" (
  echo Replace: %~nx1 =^> %PATCH%
) else (
  echo New: %~nx1 =^> %PATCH%
)
exit /b 0

:mutation_failure
echo Launch preparation stopped; original/recovery files were preserved for inspection.
set "RESULT=5"

:done
echo.
echo Launch preparation exit code: %RESULT%
pause
exit /b %RESULT%
