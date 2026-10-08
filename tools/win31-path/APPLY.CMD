@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET=" & set "RESULT=1"
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
echo Windows 3.1 path repair
echo.
set /p "TARGET=Target installed Windows 3.1 directory: "
if not defined TARGET (set "RESULT=64"& goto done)
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
if not exist "%TARGET%\WIN.COM" (echo WIN.COM was not found in "%TARGET%".& set "RESULT=2"& goto done)
if not exist "%~dp0WIN31PATH.EXE" (echo Missing WIN31PATH.EXE beside APPLY.CMD.& set "RESULT=3"& goto done)
set "PATCH=%TARGET%\PATCH"
set "REPORT=%PATCH%\PATH-REPAIR.TXT"
set "MANIFEST=%PATCH%\PATH-REPAIR.MANIFEST"
if exist "%MANIFEST%" (
  echo Existing repair manifest found; checking that this is already repaired.
  "%~dp0WIN31PATH.EXE" --root "%TARGET%" || (set "RESULT=%errorlevel%"& goto done)
  set "RESULT=0"
  goto done
)
"%~dp0WIN31PATH.EXE" --root "%TARGET%" || (set "RESULT=%errorlevel%"& goto done)
if not exist "%REPORT%" (echo No path changes were required.& set "RESULT=0"& goto done)
set "TEMP=%MANIFEST%.TMP"
set "RECORD_FAILED="
del /q "%TEMP%" 2>nul
for /f "tokens=1 delims=:" %%F in ('findstr /r /c:"^[^:][^:]*: [0-9][0-9]* .*replacement" "%REPORT%"') do call :record "%%F"
if defined RECORD_FAILED goto mutation_failure
if not exist "%TEMP%" (echo No recoverable file changes were reported.& set "RESULT=5"& goto done)
move /y "%TEMP%" "%MANIFEST%" >nul || goto mutation_failure
set "RESULT=0"
goto done

:record
set "FILE=%~1"
if not exist "%TARGET%\%FILE%" (set "RECORD_FAILED=1"& exit /b 1)
if not exist "%TARGET%\%FILE%.BAK" (set "RECORD_FAILED=1"& exit /b 1)
>>"%TEMP%" echo %FILE%
exit /b 0

:mutation_failure
del /q "%TEMP%" 2>nul
echo Path repair completed only partially; originals and temporary evidence were preserved.
set "RESULT=5"

:done
echo.
echo Path repair exit code: %RESULT%
pause
exit /b %RESULT%
