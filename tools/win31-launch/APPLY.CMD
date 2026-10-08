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
call :backup_retail "%TARGET%\SYSTEM\KRNL386.EXE" "%PATCH%\KRNL386.ORIG" FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980 88E095A7C39C6294E8E3BE71CBD2EDC1E4DCECBB7101D1581021EA7BE7B33181 || goto mutation_failure
call :backup_retail "%TARGET%\SYSTEM\WIN386.EXE" "%PATCH%\WIN386.ORIG" 6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5 C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8 || goto mutation_failure
"%~dp0PATCH386.EXE" --apply --krnl "%TARGET%\SYSTEM\KRNL386.EXE" --win386 "%TARGET%\SYSTEM\WIN386.EXE" || goto mutation_failure
call :install_mouse || goto mutation_failure
>"%PATCH%\CONFIG.NT" (
  echo dos=high, umb
  echo device=%%SystemRoot%%\system32\himem.sys
  echo files=128
  echo dosonly
)
>"%PATCH%\AUTOEXEC.NT" (
  echo @echo off
  echo SET TEMP=%%SystemRoot%%\Temp
  echo SET TMP=%%SystemRoot%%\Temp
  echo SET PATH=%TARGET%;%TARGET%\SYSTEM;%%SystemRoot%%\system32
)
"%RELEASE%\PIF.EXE" create "%PATCH%\WINSTD.PIF" --title "Windows 3.1 Standard" --program "%TARGET%\WIN.COM" --directory "%TARGET%" --arguments "/S" --config "%PATCH%\CONFIG.NT" --autoexec "%PATCH%\AUTOEXEC.NT" --close-on-exit || goto mutation_failure
"%RELEASE%\PIF.EXE" create "%PATCH%\WIN386.PIF" --title "Windows 3.1 386 Enhanced" --program "%TARGET%\WIN.COM" --directory "%TARGET%" --arguments "/3" --config "%PATCH%\CONFIG.NT" --autoexec "%PATCH%\AUTOEXEC.NT" --close-on-exit || goto mutation_failure
>"%PATCH%\WINSTD.CMD" (
  echo @echo off
  echo setlocal
  echo pushd "%%~dp0.." ^|^| exit /b 3
  echo call run16 "%%~dp0WINSTD.PIF"
  echo set result=%%errorlevel%%
  echo popd
  echo exit /b %%result%%
)
>"%PATCH%\WIN386.CMD" (
  echo @echo off
  echo setlocal
  echo pushd "%%~dp0.." ^|^| exit /b 3
  echo call run16 "%%~dp0WIN386.PIF"
  echo set result=%%errorlevel%%
  echo popd
  echo exit /b %%result%%
)
set "RESULT=0"
goto done

:backup_retail
set "CURRENT=" & set "BACKUP=" & set "RETAIL=%~3" & set "CANDIDATE=%~4"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%~1"') do set "CURRENT=%%H"
if not defined CURRENT exit /b 1
if /I "%CURRENT%"=="%RETAIL%" (
  if exist "%~2" (echo Refusing to overwrite existing recovery copy: %~2& exit /b 1)
  copy /y "%~1" "%~2" >nul || exit /b 1
  exit /b 0
)
if /I not "%CURRENT%"=="%CANDIDATE%" (echo Unsupported installed binary: %~1& exit /b 1)
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%~2"') do set "BACKUP=%%H"
if /I not "%BACKUP%"=="%RETAIL%" (echo Patched binary lacks retail recovery copy: %~2& exit /b 1)
exit /b 0

:install_mouse
set "CURRENT=" & set "RELEASE_HASH=" & set "BACKUP="
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%TARGET%\SYSTEM\MOUSE.DRV"') do set "CURRENT=%%H"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%RELEASE%\MOUSE31.DRV"') do set "RELEASE_HASH=%%H"
if not defined CURRENT exit /b 1
if /I not "%RELEASE_HASH%"=="D6BA5380A2EDCDB33E0C36850FB6BDD0542D78EC03E982DBD5114A462B0A2F1A" (
  echo Released MOUSE31.DRV does not match its approved identity.
  exit /b 1
)
if /I "%CURRENT%"=="%RELEASE_HASH%" (
  for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%PATCH%\MOUSE.DRV.ORIG"') do set "BACKUP=%%H"
  if defined BACKUP exit /b 0
  echo Installed replacement lacks its recovery copy: %PATCH%\MOUSE.DRV.ORIG
  exit /b 1
)
if exist "%PATCH%\MOUSE.DRV.ORIG" (echo Refusing to overwrite existing mouse-driver recovery copy.& exit /b 1)
copy /y "%TARGET%\SYSTEM\MOUSE.DRV" "%PATCH%\MOUSE.DRV.ORIG" >nul || exit /b 1
copy /y "%RELEASE%\MOUSE31.DRV" "%TARGET%\SYSTEM\MOUSE.DRV" >nul || exit /b 1
exit /b 0

:mutation_failure
echo Launch preparation stopped; original/recovery files were preserved for inspection.
set "RESULT=5"

:done
echo.
echo Launch preparation exit code: %RESULT%
pause
exit /b %RESULT%
