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
"%~dp0PATCHSET.EXE" --win31 "%INSTALLROOT%"
set "patch_result=%errorlevel%"
echo PATCHSET exit code: %patch_result%
rmdir "%TEMP%" 2>nul
if not defined NTVDM_SETUP_TEST_NO_PAUSE pause
exit /b %patch_result%
