@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET="
set /p "TARGET=Target prepared Windows 95 directory: "
if not defined TARGET exit /b 64
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
for %%I in ("%TARGET%") do set "TARGET=%%~sI"
set "SYSTEM=%TARGET%\SYSTEM"
set "PATCH=%TARGET%\PATCH"
if not exist "%TARGET%\SYSTEM.INI" (echo Missing SYSTEM.INI.& exit /b 2)
call :find_active_mouse
if errorlevel 1 goto restore_cores
set "TARGET_PREFIX=%TARGET%\"
call set "MOUSE_SUFFIX=%%MOUSE_PATH:%TARGET_PREFIX%=%%"
if /i "%MOUSE_SUFFIX%"=="%MOUSE_PATH%" (echo Mouse path is outside selected Windows directory.& exit /b 5)
if exist "%MOUSE_PATH%.BAK" (
  move /y "%MOUSE_PATH%.BAK" "%MOUSE_PATH%" >nul || exit /b 5
) else if exist "%MOUSE_PATH%.W95LAUNCH.BAK" (
  move /y "%MOUSE_PATH%.W95LAUNCH.BAK" "%MOUSE_PATH%" >nul || exit /b 5
)
:restore_cores
for %%F in (VMM32.VXD KRNL386.EXE) do if exist "%SYSTEM%\%%F.W95LAUNCH.BAK" move /y "%SYSTEM%\%%F.W95LAUNCH.BAK" "%SYSTEM%\%%F" >nul || exit /b 5
if exist "%PATCH%\" rmdir /s /q "%PATCH%" || exit /b 5
echo Windows 95 restoration finished.
exit /b 0

:find_active_mouse
set "IN_BOOT="
set "MOUSE_REF="
set "MOUSE_COUNT=0"
for /f "usebackq delims=" %%L in ("%TARGET%\SYSTEM.INI") do call :read_ini_line "%%L"
if not defined MOUSE_REF (
  echo SYSTEM.INI has no [boot] mouse.drv setting; refusing to select a driver.
  exit /b 1
)
if not "%MOUSE_COUNT%"=="1" (
  echo SYSTEM.INI has multiple [boot] mouse.drv settings; refusing to select a driver.
  exit /b 1
)
set "MOUSE_REF=%MOUSE_REF: =%"
if not defined MOUSE_REF exit /b 1
if /i "%MOUSE_REF:~0,7%"=="SYSTEM\" (
  set "MOUSE_PATH=%TARGET%\%MOUSE_REF%"
) else if "%MOUSE_REF:\=%"=="%MOUSE_REF%" (
  set "MOUSE_PATH=%SYSTEM%\%MOUSE_REF%"
) else (
  for %%I in ("%MOUSE_REF%") do set "MOUSE_PATH=%%~fI"
)
exit /b 0

:read_ini_line
set "INI_LINE=%~1"
set "INI_HEADER=%INI_LINE: =%"
if /i "%INI_HEADER%"=="[boot]" set "IN_BOOT=1"
if "%INI_HEADER:~0,1%"=="[" if /i not "%INI_HEADER%"=="[boot]" set "IN_BOOT="
if not defined IN_BOOT exit /b 0
for /f "tokens=1,* delims==" %%A in ("%INI_LINE%") do if /i "%%A"=="mouse.drv" (
  set "MOUSE_REF=%%B"
  set /a MOUSE_COUNT+=1
)
exit /b 0
