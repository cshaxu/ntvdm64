@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET=%~1"
if not defined TARGET set /p "TARGET=Installed Windows directory: "
if not defined TARGET exit /b 64
for %%I in ("%TARGET%") do set "TARGET=%%~sI"
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
set "PATCH=%TARGET%\PATCH"
set "INI=%TARGET%\SYSTEM.INI"
for %%F in (MULTITASK.INI.BAK MULTITASK.INI.NEW MULTITASK.HASH INSIGNIA.386 NTVDM64.386) do if not exist "%PATCH%\%%F" (echo Missing installation record: %%F.& exit /b 5)
if not exist "%RELEASE%\HASH.EXE" (echo Missing released HASH.EXE.& exit /b 3)
fc /b "%INI%" "%PATCH%\MULTITASK.INI.NEW" >nul || (echo SYSTEM.INI changed since installation; refusing restore.& exit /b 5)
set "EXPECTED=" & set "ACTUAL="
set /p "EXPECTED=" <"%PATCH%\MULTITASK.HASH"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%PATCH%\MULTITASK.INI.BAK"') do set "ACTUAL=%%H"
if not defined EXPECTED exit /b 5
if /i not "%EXPECTED%"=="%ACTUAL%" (echo Backup hash mismatch; refusing restore.& exit /b 5)
for %%F in (INSIGNIA.386 NTVDM64.386) do fc /b "%PATCH%\%%F" "%RELEASE%\%%F" >nul || (echo Driver changed; refusing restore.& exit /b 5)
copy /b /y "%PATCH%\MULTITASK.INI.BAK" "%INI%" >nul || exit /b 5
fc /b "%PATCH%\MULTITASK.INI.BAK" "%INI%" >nul || exit /b 5
for %%F in (INSIGNIA.386 NTVDM64.386 MULTITASK.INI.NEW MULTITASK.HASH MULTITASK.INI.BAK) do (
  del /q "%PATCH%\%%F" || exit /b 5
)
echo Multitask configuration restored.
exit /b 0
