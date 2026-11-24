@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~4"=="" exit /b 2
if not "%~5"=="" exit /b 2
if not "%~4"=="31" if not "%~4"=="95" exit /b 2
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
if "%~4"=="95" if not exist "%DEST%\PATCH\WIN.PIF" exit /b 2
set "FAILED="
for /r "%DEST%" %%F in (*.INI *.CMD *.NT) do if exist "%%F" if not defined FAILED (
  "%ROOT%\assets\release\PATH95.EXE" replace-text "%%F" "%TASK%\path-rebase.candidate" "%SOURCE%" "%DEST%"
  if errorlevel 1 (
    set "FAILED=1"
  ) else (
    copy /b /y "%TASK%\path-rebase.candidate" "%%F" >nul
    if errorlevel 1 (
      set "FAILED=1"
    ) else (
      del /q "%TASK%\path-rebase.candidate"
      if exist "%TASK%\path-rebase.candidate" set "FAILED=1"
    )
  )
)
if defined FAILED exit /b 1
if "%~4"=="31" for /r "%DEST%" %%F in (*.PIF) do if exist "%%F" if not defined FAILED (
  "%ROOT%\assets\release\PIF.EXE" replace-root "%%F" --from "%SOURCE%" --to "%DEST%"
  if errorlevel 1 set "FAILED=1"
)
if defined FAILED exit /b 1
if "%~4"=="95" (
  "%ROOT%\assets\release\PIF.EXE" replace-root "%DEST%\PATCH\WIN.PIF" --from "%SOURCE%" --to "%DEST%"
  if errorlevel 1 set "FAILED=1"
)
if defined FAILED exit /b 1
for %%F in ("%DEST%\*.GRP") do if exist "%%F" if not defined FAILED (
  "%ROOT%\assets\release\GRP.EXE" replace-root "%%F" --root "%DEST%"
  if errorlevel 1 set "FAILED=1"
)
if defined FAILED exit /b 1
exit /b 0
