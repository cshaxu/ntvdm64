@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TEMP=%~dp0TEMP"
set "TMP=%TEMP%"
if not exist "%TEMP%\" mkdir "%TEMP%" || (
  echo Cannot create Setup temporary directory.
  if not defined NTVDM_SETUP_TEST_NO_PAUSE pause
  exit /b 4
)
where run16 >nul 2>nul || (
  echo run16.exe must be available through PATH.
  rmdir "%TEMP%" 2>nul
  if not defined NTVDM_SETUP_TEST_NO_PAUSE pause
  exit /b 2
)
pushd "%~dp0.." || (
  echo Cannot enter the original Setup media directory.
  rmdir "%TEMP%" 2>nul
  if not defined NTVDM_SETUP_TEST_NO_PAUSE pause
  exit /b 3
)
call run16 command /c SETUP.EXE
set "setup_result=%errorlevel%"
popd
echo Original Setup/run16 exit code: %setup_result%
if not "%setup_result%"=="0" (
  echo Setup did not report success.  No installed PATCH has been generated.
  rmdir "%TEMP%" 2>nul
  if not defined NTVDM_SETUP_TEST_NO_PAUSE pause
  exit /b %setup_result%
)
set /p "INSTALLROOT=Windows 3.1 directory selected in Setup: "
if "%INSTALLROOT%"=="" (
  echo No installation directory supplied.
  rmdir "%TEMP%" 2>nul
  if not defined NTVDM_SETUP_TEST_NO_PAUSE pause
  exit /b 4
)
if not exist "%INSTALLROOT%\WIN.COM" (
  echo The selected directory does not contain WIN.COM. Setup may have been cancelled or is incomplete.
  echo No installed PATCH has been generated.
  rmdir "%TEMP%" 2>nul
  if not defined NTVDM_SETUP_TEST_NO_PAUSE pause
  exit /b 5
)
for %%I in ("%INSTALLROOT%") do set "INSTALLROOT=%%~fI"
set "DEST=%INSTALLROOT%\PATCH"
if not exist "%~dp0PIF.EXE" (echo Missing PIF.EXE.& exit /b 3)
for %%F in (SYSTEM\DOSX.EXE SYSTEM\KRNL386.EXE SYSTEM\WIN386.EXE SYSTEM.INI) do if not exist "%INSTALLROOT%\%%F" (echo Missing installed file: %%F& exit /b 5)
for %%F in (WIN.CMD PIF.EXE INSIGNIA.386 NTVDM64.386) do if not exist "%~dp0%%F" (echo Missing setup component: %%F& exit /b 3)
if not exist "%DEST%\" mkdir "%DEST%" || exit /b 4
for %%F in (WIN.CMD PIF.EXE INSIGNIA.386 NTVDM64.386) do copy /y "%~dp0%%F" "%DEST%\%%F" >nul || exit /b 5
call "%DEST%\WIN.CMD" --install
set "patch_result=%errorlevel%"
if "%patch_result%"=="0" for %%F in (WINSTD.CMD WIN386.CMD WINSTD.PIF WIN386.PIF) do if exist "%DEST%\%%F" del /q "%DEST%\%%F" || exit /b 5
if "%patch_result%"=="0" echo Prepared %DEST%\WIN.CMD. Choose Windows mode and DOSONLY each time you launch.
rmdir "%TEMP%" 2>nul
if not defined NTVDM_SETUP_TEST_NO_PAUSE pause
exit /b %patch_result%
