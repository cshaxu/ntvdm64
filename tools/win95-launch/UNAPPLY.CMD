@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET=%~1"
if not defined TARGET set /p "TARGET=Target prepared Windows 95 directory: "
if not defined TARGET exit /b 64
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
for %%I in ("%TARGET%") do set "TARGET=%%~sI"
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
set "SYSTEM=%TARGET%\SYSTEM"
set "PATCH=%TARGET%\PATCH"
set "JOURNAL=%PATCH%\W95LAUNCH.MANIFEST"
if not exist "%JOURNAL%" (
  echo Missing Win95 Launch journal: %JOURNAL%
  exit /b 5
)
set "SCHEMA="
set "MOUSE_PATH="
set "MOUSE_HASH="
set "VMM_HASH="
set "KERNEL_HASH="
set "SETVER_HASH="
for /f "usebackq tokens=1,* delims==" %%A in ("%JOURNAL%") do (
  if /i "%%A"=="schema" set "SCHEMA=%%B"
  if /i "%%A"=="mouse" set "MOUSE_PATH=%%B"
  if /i "%%A"=="mousehash" set "MOUSE_HASH=%%B"
  if /i "%%A"=="vmmhash" set "VMM_HASH=%%B"
  if /i "%%A"=="kernelhash" set "KERNEL_HASH=%%B"
  if /i "%%A"=="setverhash" set "SETVER_HASH=%%B"
)
if not "%SCHEMA%"=="1" if not "%SCHEMA%"=="2" if not "%SCHEMA%"=="3" goto bad_journal
if "%SCHEMA%"=="3" if not defined SETVER_HASH goto bad_journal
if "%SCHEMA%"=="3" if not defined KERNEL_HASH goto bad_journal
if "%SCHEMA%"=="2" if not defined KERNEL_HASH goto bad_journal
if not defined MOUSE_PATH goto bad_journal
if not defined MOUSE_HASH goto bad_journal
if not defined VMM_HASH goto bad_journal
set "TARGET_PREFIX=%TARGET%\"
call set "MOUSE_SUFFIX=%%MOUSE_PATH:%TARGET_PREFIX%=%%"
if /i "%MOUSE_SUFFIX%"=="%MOUSE_PATH%" goto bad_journal
if not exist "%MOUSE_PATH%" goto preserve
if not exist "%MOUSE_PATH%.W95LAUNCH.BAK" goto preserve
if not exist "%SYSTEM%\VMM32.VXD" goto preserve
if not exist "%SYSTEM%\VMM32.VXD.W95LAUNCH.BAK" goto preserve
if defined KERNEL_HASH if not exist "%SYSTEM%\KRNL386.EXE" goto preserve
if defined KERNEL_HASH if not exist "%SYSTEM%\KRNL386.EXE.W95LAUNCH.BAK" goto preserve
fc /b "%MOUSE_PATH%" "%RELEASE%\MOUSE31.DRV" >nul || goto preserve
if exist "%PATCH%\compat95.sys" fc /b "%PATCH%\compat95.sys" "%RELEASE%\compat95.sys" >nul || goto preserve
if not exist "%RELEASE%\HASH.EXE" goto preserve
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%MOUSE_PATH%"') do set "CURRENT_MOUSE_HASH=%%H"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%SYSTEM%\VMM32.VXD"') do set "CURRENT_VMM_HASH=%%H"
if /i not "%CURRENT_MOUSE_HASH%"=="%MOUSE_HASH%" goto preserve
if /i not "%CURRENT_VMM_HASH%"=="%VMM_HASH%" goto preserve
if defined KERNEL_HASH for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%SYSTEM%\KRNL386.EXE"') do set "CURRENT_KERNEL_HASH=%%H"
if defined KERNEL_HASH call :check_kernel || goto preserve
if defined SETVER_HASH if not exist "%PATCH%\SETVER.EXE" goto preserve
if defined SETVER_HASH for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%PATCH%\SETVER.EXE"') do set "CURRENT_SETVER_HASH=%%H"
if defined SETVER_HASH call :check_setver || goto preserve

if defined KERNEL_HASH (
  del /q "%SYSTEM%\KRNL386.EXE" || goto restore_fail
  move /y "%SYSTEM%\KRNL386.EXE.W95LAUNCH.BAK" "%SYSTEM%\KRNL386.EXE" >nul || goto restore_fail
)
echo Restore: %MOUSE_PATH%
del /q "%MOUSE_PATH%" || goto restore_fail
move /y "%MOUSE_PATH%.W95LAUNCH.BAK" "%MOUSE_PATH%" >nul || goto restore_fail
echo Restore: %SYSTEM%\VMM32.VXD
del /q "%SYSTEM%\VMM32.VXD" || goto restore_fail
move /y "%SYSTEM%\VMM32.VXD.W95LAUNCH.BAK" "%SYSTEM%\VMM32.VXD" >nul || goto restore_fail
if defined SETVER_HASH del /q "%PATCH%\SETVER.EXE" || goto restore_fail
for %%F in (CONFIG.NT AUTOEXEC.NT WIN.PIF WIN.CMD compat95.sys W95LAUNCH.MANIFEST) do if exist "%PATCH%\%%F" del /q "%PATCH%\%%F" || goto restore_fail
rmdir "%PATCH%\TEMP" 2>nul
rmdir "%PATCH%" 2>nul
echo Win95 launcher restored.
exit /b 0

:bad_journal
echo Invalid journal; no files changed.
exit /b 5
:preserve
echo Refusing to overwrite a changed or incomplete Win95 Launch installation.
exit /b 5
:restore_fail
echo Recovery stopped; remaining files were preserved for inspection.
exit /b 5

:check_kernel
if /i not "%CURRENT_KERNEL_HASH%"=="%KERNEL_HASH%" exit /b 1
exit /b 0

:check_setver
if /i not "%CURRENT_SETVER_HASH%"=="%SETVER_HASH%" exit /b 1
exit /b 0
