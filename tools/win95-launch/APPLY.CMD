@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET="
set /p "TARGET=Target installed Windows 95 directory: "
if not defined TARGET exit /b 64
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
for %%I in ("%TARGET%") do set "TARGET=%%~sI"
if not "%TARGET: =%"=="%TARGET%" (
  echo Win95 root needs a short path without spaces for DOS device configuration.
  exit /b 5
)
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
set "SYSTEM=%TARGET%\SYSTEM"
set "PATCH=%TARGET%\PATCH"
set "LOGO_OPTION="
if exist "%PATCH%\LOGO.SYS" set "LOGO_OPTION= /LOGO=%PATCH%\LOGO.SYS"

for %%F in ("%TARGET%\WIN.COM" "%SYSTEM%\VMM32.VXD" "%SYSTEM%\KRNL386.EXE" "%TARGET%\SYSTEM.INI") do if not exist "%%~F" (
  echo Missing Win95 installation file: %%~F
  exit /b 2
)
for %%F in (patch95.exe compat95.sys PIF.EXE MOUSE31.DRV) do if not exist "%RELEASE%\%%F" (
  echo Missing released component: %RELEASE%\%%F
  exit /b 3
)
for %%F in (CONFIG.NT AUTOEXEC.NT WIN.PIF WIN.CMD compat95.sys SETVER.EXE VMM32.W95NEW KERNEL.W95NEW TEMP) do if exist "%PATCH%\%%F" (
  echo Refusing to overwrite pre-existing PATCH content: %%F
  exit /b 5
)

set "RUN16="
for /f "delims=" %%I in ('where run16.exe 2^>nul') do if not defined RUN16 set "RUN16=%%I"
if not defined RUN16 (
  echo Missing run16.exe on PATH.
  exit /b 3
)
for %%I in ("%RUN16%") do set "RUNTIME=%%~dpI"
if not exist "%RUNTIME%SETVER.EXE" (
  echo Missing SETVER.EXE beside %RUN16%.
  exit /b 3
)
call :find_active_mouse || exit /b 5
set "TARGET_PREFIX=%TARGET%\"
call set "MOUSE_SUFFIX=%%MOUSE_PATH:%TARGET_PREFIX%=%%"
if /i "%MOUSE_SUFFIX%"=="%MOUSE_PATH%" (
  echo SYSTEM.INI mouse.drv escapes the selected Windows tree.
  exit /b 5
)
if not exist "%MOUSE_PATH%" (
  echo SYSTEM.INI [boot] mouse.drv does not name an existing driver: %MOUSE_PATH%
  exit /b 5
)
if exist "%MOUSE_PATH%.BAK" (
  echo Existing mouse recovery copy; refusing to take over a prior preparation.
  exit /b 5
)
if exist "%SYSTEM%\KRNL386.EXE.W95LAUNCH.BAK" (
  echo Existing kernel recovery copy.
  exit /b 5
)
if exist "%SYSTEM%\VMM32.VXD.W95LAUNCH.BAK" (
  echo Existing VMM32 recovery copy; refusing to take over a prior preparation.
  exit /b 5
)

if not exist "%PATCH%\" (
  mkdir "%PATCH%" || exit /b 4
  set "NEW_PATCH=1"
)
if not exist "%PATCH%\TEMP\" (
  mkdir "%PATCH%\TEMP" || goto fail
  set "NEW_TEMP=1"
)
"%RELEASE%\patch95.exe" prepare "%SYSTEM%\VMM32.VXD" "%PATCH%\VMM32.W95NEW" || goto fail
if not exist "%PATCH%\VMM32.W95NEW" goto fail
"%RELEASE%\patch95.exe" prepare-kernel "%SYSTEM%\KRNL386.EXE" "%PATCH%\KERNEL.W95NEW" || goto fail
if not exist "%PATCH%\KERNEL.W95NEW" goto fail
"%RELEASE%\patch95.exe" prepare-setver "%RUNTIME%SETVER.EXE" "%PATCH%\SETVER.EXE" || goto fail

move /y "%SYSTEM%\VMM32.VXD" "%SYSTEM%\VMM32.VXD.W95LAUNCH.BAK" >nul || goto fail
set "NEW_VMM_BACKUP=1"
move /y "%PATCH%\VMM32.W95NEW" "%SYSTEM%\VMM32.VXD" >nul || goto fail
move /y "%SYSTEM%\KRNL386.EXE" "%SYSTEM%\KRNL386.EXE.W95LAUNCH.BAK" >nul || goto fail
set "NEW_KERNEL_BACKUP=1"
move /y "%PATCH%\KERNEL.W95NEW" "%SYSTEM%\KRNL386.EXE" >nul || goto fail
copy /y "%RELEASE%\compat95.sys" "%PATCH%\compat95.sys" >nul || goto fail

move /y "%MOUSE_PATH%" "%MOUSE_PATH%.BAK" >nul || goto fail
set "NEW_MOUSE_BACKUP=1"
copy /y "%RELEASE%\MOUSE31.DRV" "%MOUSE_PATH%" >nul || goto fail

>"%PATCH%\CONFIG.NT" (
  echo dos=high, umb
  echo device=%%SystemRoot%%\system32\himem.sys
  echo device=%PATCH%\SETVER.EXE
  echo files=128
  echo device=%TARGET%\IFSHLP.SYS
  echo device=%PATCH%\compat95.sys /ROOT=%TARGET%%LOGO_OPTION%
)
>"%PATCH%\AUTOEXEC.NT" (
  echo @echo off
  echo SET windir=%TARGET%
  echo SET winbootdir=%TARGET%
  echo SET TEMP=%PATCH%\TEMP
  echo SET TMP=%PATCH%\TEMP
  echo SET PATH=%TARGET%;%SYSTEM%;%%SystemRoot%%\system32
)
"%RELEASE%\PIF.EXE" create "%PATCH%\WIN.PIF" --title "Windows 95" --program "%TARGET%\WIN.COM" --directory "%TARGET%" --arguments "" --config "%PATCH%\CONFIG.NT" --autoexec "%PATCH%\AUTOEXEC.NT" --close-on-exit --w386-max-xms 65535 --w386-flags 0x1003 --nt31-flags 1 || goto fail
>"%PATCH%\WIN.CMD" (
  echo @echo off
  echo call "%RUN16%" "%%~dp0WIN.PIF"
)
echo Win95 launcher prepared: %PATCH%\WIN.CMD
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

:fail
echo Launch preparation failed; restoring only this run's replacements.
if defined NEW_MOUSE_BACKUP if exist "%MOUSE_PATH%.BAK" (
  del /q "%MOUSE_PATH%" >nul 2>nul
  move /y "%MOUSE_PATH%.BAK" "%MOUSE_PATH%" >nul
)
if defined NEW_KERNEL_BACKUP if exist "%SYSTEM%\KRNL386.EXE.W95LAUNCH.BAK" (
  del /q "%SYSTEM%\KRNL386.EXE" >nul 2>nul
  move /y "%SYSTEM%\KRNL386.EXE.W95LAUNCH.BAK" "%SYSTEM%\KRNL386.EXE" >nul
)
if defined NEW_VMM_BACKUP if exist "%SYSTEM%\VMM32.VXD.W95LAUNCH.BAK" (
  del /q "%SYSTEM%\VMM32.VXD" >nul 2>nul
  move /y "%SYSTEM%\VMM32.VXD.W95LAUNCH.BAK" "%SYSTEM%\VMM32.VXD" >nul
)
for %%F in (VMM32.W95NEW KERNEL.W95NEW SETVER.EXE CONFIG.NT AUTOEXEC.NT WIN.PIF WIN.CMD compat95.sys) do if exist "%PATCH%\%%F" del /q "%PATCH%\%%F" >nul 2>nul
if defined NEW_TEMP rmdir "%PATCH%\TEMP" 2>nul
if defined NEW_PATCH rmdir "%PATCH%" 2>nul
exit /b 5
