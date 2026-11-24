@echo off
setlocal EnableExtensions DisableDelayedExpansion
if not "%~4"=="" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "DEST=%~f1"
set "SOURCE=%~f2"
set "TASK=%~f3"
for %%I in ("%DEST%\..") do if /i not "%%~fI"=="O:\tmp" exit /b 2
if not exist "%DEST%\.ntvdm64-guest-owner" exit /b 2
set "OWNER="
set /p "OWNER="<"%DEST%\.ntvdm64-guest-owner"
if /i not "%OWNER%"=="%TASK%" exit /b 2
if not exist "%TASK%\" exit /b 2
for %%H in (SYSTEM.DAT USER.DAT) do (
  "%ROOT%\assets\release\PATH95.EXE" replace-equal "%DEST%\%%H" "%TASK%\%%H.CANDIDATE" "%SOURCE%" "%DEST%"
  if errorlevel 1 exit /b 1
)
for %%H in (SYSTEM.DAT USER.DAT) do (
  attrib -r -s -h "%DEST%\%%H"
  if errorlevel 1 exit /b 1
  copy /b /y "%TASK%\%%H.CANDIDATE" "%DEST%\%%H" >nul
  if errorlevel 1 exit /b 1
)
exit /b 0
