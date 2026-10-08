@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET=" & set "RESULT=1" & set "FAILED="
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
echo Windows 3.1 path repair
echo.
set /p "TARGET=Target installed Windows 3.1 directory: "
if not defined TARGET (set "RESULT=64"& goto done)
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
if not exist "%TARGET%\WIN.COM" (echo WIN.COM was not found in "%TARGET%".& set "RESULT=2"& goto done)
for %%F in (WIN31PATH.EXE) do if not exist "%~dp0%%F" (echo Missing %%F beside APPLY.CMD.& set "RESULT=3"& goto done)
for %%F in (GRP.EXE PIF.EXE) do if not exist "%RELEASE%\%%F" (echo Missing released add-on: %RELEASE%\%%F& set "RESULT=3"& goto done)
set "PATCH=%TARGET%\PATCH"
set "MANIFEST=%PATCH%\PATH-REPAIR.MANIFEST"
for /f "tokens=1,* delims==" %%A in ('call "%~dp0WIN31PATH.EXE" --discover-root "%TARGET%"') do (
  if /I "%%A"=="OLD_ROOT" set "OLD_ROOT=%%B"
  if /I "%%A"=="NEW_ROOT" set "NEW_ROOT=%%B"
)
if not defined OLD_ROOT (echo Could not prove the current Windows root from PROGMAN.INI.& set "RESULT=3"& goto done)
echo Current installed path: %OLD_ROOT%
echo Requested installed path: %TARGET%
if /I "%OLD_ROOT%"=="%TARGET%" (echo Paths already match.& set "RESULT=0"& goto done)
if exist "%MANIFEST%" (echo Existing repair manifest found; use UNAPPLY.CMD before another repair.& set "RESULT=5"& goto done)
if not exist "%PATCH%" mkdir "%PATCH%" || (echo Could not create "%PATCH%".& set "RESULT=4"& goto done)
set "TEMP=%MANIFEST%.TMP"
del /q "%TEMP%" 2>nul
for %%F in ("%TARGET%\*.INI") do if exist "%%~fF" call :repair_ini "%%~fF"
for %%F in ("%TARGET%\*.PIF") do if exist "%%~fF" call :repair_pif "%%~fF"
for %%F in ("%TARGET%\*.GRP") do if exist "%%~fF" call :repair_grp "%%~fF"
if defined FAILED goto mutation_failure
if exist "%TEMP%" (move /y "%TEMP%" "%MANIFEST%" >nul || goto mutation_failure) else echo No in-tree path references required repair.
set "RESULT=0"
goto done

:repair_ini
set "SOURCE=%~1" & set "WORK=%~1.T438"
findstr /i /c:"%OLD_ROOT%" "%SOURCE%" >nul || exit /b 0
findstr /c:"!" "%SOURCE%" >nul && (echo Cannot safely rewrite ^! in INI: %~nx1& set "FAILED=1"& exit /b 1)
call :rewrite_ini "%SOURCE%" "%WORK%" || (set "FAILED=1"& exit /b 1)
fc /b "%SOURCE%" "%WORK%" >nul
if not errorlevel 1 (del /q "%WORK%"& echo Case-only or unsupported INI reference: %~nx1& set "FAILED=1"& exit /b 1)
call :backup "%SOURCE%" || (del /q "%WORK%"& set "FAILED=1"& exit /b 1)
move /y "%WORK%" "%SOURCE%" >nul || (set "FAILED=1"& exit /b 1)
call :record "%~nx1"
echo INI: %~nx1
exit /b 0

:rewrite_ini
setlocal EnableDelayedExpansion
>"%~2" (
  for /f "usebackq delims=" %%L in ("%~1") do (
    set "LINE=%%L"
    set "LINE=!LINE:%OLD_ROOT%=%TARGET%!"
    echo(!LINE!
  )
)
endlocal
exit /b 0

:repair_pif
set "SOURCE=%~1" & set "CHANGED="
"%RELEASE%\PIF.EXE" show "%SOURCE%" | findstr /i /c:"PROGRAM=%OLD_ROOT%" /c:"DIRECTORY=%OLD_ROOT%" /c:"CONFIG=%OLD_ROOT%" /c:"AUTOEXEC=%OLD_ROOT%" >nul || exit /b 0
call :backup "%SOURCE%" || (set "FAILED=1"& exit /b 1)
for /f "tokens=1,2 delims==" %%A in ('call "%RELEASE%\PIF.EXE" replace-root "%SOURCE%" --from "%OLD_ROOT%" --to "%TARGET%"') do if /I "%%A"=="CHANGED" set "CHANGED=%%B"
if not "%CHANGED%"=="1" (echo Could not rewrite PIF: %~nx1& set "FAILED=1"& exit /b 1)
call :record "%~nx1"
echo PIF: %~nx1
exit /b 0

:repair_grp
set "SOURCE=%~1" & set "CHANGED="
call :backup "%SOURCE%" || (set "FAILED=1"& exit /b 1)
for /f "tokens=1,2 delims==" %%A in ('call "%RELEASE%\GRP.EXE" replace-root "%SOURCE%" --root "%TARGET%"') do if /I "%%A"=="CHANGED" set "CHANGED=%%B"
if "%CHANGED%"=="0" (del /q "%SOURCE%.BAK"& exit /b 0)
if not defined CHANGED (echo Could not rewrite GRP: %~nx1& set "FAILED=1"& exit /b 1)
call :record "%~nx1"
echo GRP: %~nx1
exit /b 0

:backup
if exist "%~1.BAK" (echo Existing backup blocks replacement: %~nx1.BAK& exit /b 1)
copy /y "%~1" "%~1.BAK" >nul || exit /b 1
exit /b 0

:record
>>"%TEMP%" echo %~1
exit /b 0

:mutation_failure
del /q "%TEMP%" 2>nul
echo Path repair stopped; changed files retain adjacent .BAK copies for recovery.
set "RESULT=5"

:done
echo.
echo Path repair exit code: %RESULT%
pause
exit /b %RESULT%
