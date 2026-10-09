@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET="
set "RESULT=1"
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
echo Windows 3.1 NTVDM launch recovery
echo.
set /p "TARGET=Target prepared Windows 3.1 directory: "
if not defined TARGET (set "RESULT=64"& goto done)
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
for %%F in (SYSTEM\KRNL386.EXE SYSTEM\KRNL386.EXE.BAK SYSTEM\WIN386.EXE SYSTEM\WIN386.EXE.BAK SYSTEM\MOUSE.DRV SYSTEM\MOUSE.DRV.BAK) do if not exist "%TARGET%\%%F" (
  echo Missing prepared Windows file: %%F
  set "RESULT=2"
  goto done
)
for %%F in (HASH.EXE MOUSE31.DRV) do if not exist "%RELEASE%\%%F" (
  echo Missing released add-on: %RELEASE%\%%F
  set "RESULT=3"
  goto done
)
call :verify_binary "%TARGET%\SYSTEM\KRNL386.EXE" FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980 88E095A7C39C6294E8E3BE71CBD2EDC1E4DCECBB7101D1581021EA7BE7B33181 || goto mismatch
call :verify_binary "%TARGET%\SYSTEM\WIN386.EXE" 6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5 C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8 || goto mismatch
call :verify_mouse || goto mismatch
echo Restore: KRNL386.EXE =^> %TARGET%\SYSTEM
move /y "%TARGET%\SYSTEM\KRNL386.EXE.BAK" "%TARGET%\SYSTEM\KRNL386.EXE" >nul || goto mutation_failure
echo Restore: WIN386.EXE =^> %TARGET%\SYSTEM
move /y "%TARGET%\SYSTEM\WIN386.EXE.BAK" "%TARGET%\SYSTEM\WIN386.EXE" >nul || goto mutation_failure
echo Restore: MOUSE.DRV =^> %TARGET%\SYSTEM
move /y "%TARGET%\SYSTEM\MOUSE.DRV.BAK" "%TARGET%\SYSTEM\MOUSE.DRV" >nul || goto mutation_failure
set "PATCH=%TARGET%\PATCH"
if exist "%PATCH%\" (
  rem These six names are created only by APPLY.CMD. Preserve all other owner content.
  for %%F in (CONFIG.NT AUTOEXEC.NT WINSTD.PIF WIN386.PIF WINSTD.CMD WIN386.CMD) do if exist "%PATCH%\%%F" (
    echo Remove: %%F =^> %PATCH%
    del /q "%PATCH%\%%F"
  )
  rmdir "%PATCH%" 2>nul
)
set "RESULT=0"
goto done

:verify_binary
set "CURRENT=" & set "BACKUP=" & set "RETAIL=%~2" & set "CANDIDATE=%~3"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%~1"') do set "CURRENT=%%H"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%~1.BAK"') do set "BACKUP=%%H"
if /I not "%CURRENT%"=="%CANDIDATE%" exit /b 1
if /I not "%BACKUP%"=="%RETAIL%" exit /b 1
exit /b 0

:verify_mouse
set "CURRENT=" & set "RELEASE_HASH=" & set "BACKUP="
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%TARGET%\SYSTEM\MOUSE.DRV"') do set "CURRENT=%%H"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%RELEASE%\MOUSE31.DRV"') do set "RELEASE_HASH=%%H"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%TARGET%\SYSTEM\MOUSE.DRV.BAK"') do set "BACKUP=%%H"
if not defined BACKUP exit /b 1
if /I not "%CURRENT%"=="%RELEASE_HASH%" exit /b 1
exit /b 0

:mismatch
echo Refusing to restore an unrecognised current or backup file.
set "RESULT=5"
goto done

:mutation_failure
echo Recovery stopped; remaining files were preserved for inspection.
set "RESULT=5"

:done
echo.
echo Launch recovery exit code: %RESULT%
pause
exit /b %RESULT%
