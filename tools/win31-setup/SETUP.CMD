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
if not exist "%DEST%\" mkdir "%DEST%" || exit /b 4
if not exist "%DEST%\TEMP\" mkdir "%DEST%\TEMP" || exit /b 4
set "WORK=%DEST%\W31PROFILES"
if exist "%WORK%\" (echo Preserve existing profile staging directory.& exit /b 5)
mkdir "%WORK%" || exit /b 4
set "patch_result=6"
>"%WORK%\CONFIG.NT" echo REM Windows 3.1 private DOS profile
>>"%WORK%\CONFIG.NT" echo dos=high, umb
>>"%WORK%\CONFIG.NT" echo device=%%SystemRoot%%\system32\himem.sys
>>"%WORK%\CONFIG.NT" echo files=128
>"%WORK%\AUTOEXEC.NT" echo @echo off
>>"%WORK%\AUTOEXEC.NT" echo SET TEMP=%DEST%\TEMP
>>"%WORK%\AUTOEXEC.NT" echo SET TMP=%DEST%\TEMP
>>"%WORK%\AUTOEXEC.NT" echo SET PATH=%INSTALLROOT%;%INSTALLROOT%\SYSTEM;%%SystemRoot%%\system32
"%~dp0PIF.EXE" create "%WORK%\WINSTD.PIF" --title "Windows 3.1 Standard" --program "%INSTALLROOT%\WIN.COM" --directory "%INSTALLROOT%" --arguments "/S" --config "%DEST%\CONFIG.NT" --autoexec "%DEST%\AUTOEXEC.NT" --close-on-exit --w386-max-xms 65535 --w386-flags 0x1003 --nt31-flags 1 || goto profile_cleanup
"%~dp0PIF.EXE" create "%WORK%\WIN386.PIF" --title "Windows 3.1 386 Enhanced" --program "%INSTALLROOT%\WIN.COM" --directory "%INSTALLROOT%" --arguments "/3" --config "%DEST%\CONFIG.NT" --autoexec "%DEST%\AUTOEXEC.NT" --close-on-exit --w386-max-xms 65535 --w386-flags 0x1003 --nt31-flags 1 || goto profile_cleanup
for %%M in (WINSTD WIN386) do call :make_launch %%M || goto profile_cleanup
for %%F in (CONFIG.NT AUTOEXEC.NT WINSTD.PIF WIN386.PIF WINSTD.CMD WIN386.CMD) do if exist "%DEST%\%%F" fc /b "%DEST%\%%F" "%WORK%\%%F" >nul || (echo Preserve changed profile: %%F& set "patch_result=5"& goto profile_cleanup)
for %%F in (CONFIG.NT AUTOEXEC.NT WINSTD.PIF WIN386.PIF WINSTD.CMD WIN386.CMD) do copy /y "%WORK%\%%F" "%DEST%\%%F" >nul || goto profile_cleanup
set "patch_result=0"
echo Prepared %DEST%
:profile_cleanup
for %%F in (CONFIG.NT AUTOEXEC.NT WINSTD.PIF WIN386.PIF WINSTD.CMD WIN386.CMD) do if exist "%WORK%\%%F" del /q "%WORK%\%%F"
rmdir "%WORK%" 2>nul
rmdir "%TEMP%" 2>nul
if not defined NTVDM_SETUP_TEST_NO_PAUSE pause
exit /b %patch_result%
:make_launch
>"%WORK%\%~1.CMD" echo @echo off
>>"%WORK%\%~1.CMD" echo setlocal
>>"%WORK%\%~1.CMD" echo where run16 ^>nul 2^>nul ^|^| exit /b 2
>>"%WORK%\%~1.CMD" echo set "TEMP=%%~dp0TEMP"
>>"%WORK%\%~1.CMD" echo set "TMP=%%TEMP%%"
>>"%WORK%\%~1.CMD" echo if not exist "%%TEMP%%\" mkdir "%%TEMP%%" ^|^| exit /b 4
>>"%WORK%\%~1.CMD" echo pushd "%%~dp0.." ^|^| exit /b 3
>>"%WORK%\%~1.CMD" echo call run16 "%%~dp0%~1.PIF"
>>"%WORK%\%~1.CMD" echo set result=%%errorlevel%%
>>"%WORK%\%~1.CMD" echo popd
>>"%WORK%\%~1.CMD" echo exit /b %%result%%
exit /b 0
