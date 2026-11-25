@echo off
rem DEST [--close-on-exit] | DEST pure [--oem-paths|--literal-startup]
rem DEST xms GUEST_ROOT [--default-int15|--default-low|--reserved-high]
setlocal EnableExtensions DisableDelayedExpansion
if "%~1"=="" exit /b 2
if not "%~5"=="" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "DEST=%~1"
if not exist "%DEST%\" exit /b 2
set "PIF=%ROOT%\assets\release\PIF.EXE"
if defined NTVDM_TEST_PIF_EXE set "PIF=%NTVDM_TEST_PIF_EXE%"
if not exist "%PIF%" exit /b 2
set "TEMPLATE=%ROOT%\test\observation\fixtures\memory-profile.pif"
set "CLOSE="
set "DIRECTORY="
set "CONFIG_ROOT=%DEST%"
if "%~2"=="pure" goto pure
if "%~2"=="xms" goto xms
if not "%~2"=="" if not "%~2"=="--close-on-exit" exit /b 2
if not "%~3"=="" exit /b 2
set "STEM=E30"
set "TITLE=S30 EMS capability"
set "PROGRAM=E30.COM"
set "MEMORY=--w386-min-ems 1024"
set "CLOSE=%~2"
goto prepare
:pure
if not "%~4"=="" exit /b 2
if not "%~3"=="" if not "%~3"=="--oem-paths" if not "%~3"=="--literal-startup" exit /b 2
set "STEM=P30"
set "TITLE=S30 pure DOS capability"
set "PROGRAM=P30.COM"
set "MEMORY=--w386-min-ems 0 --w386-min-xms 1024"
if "%~3"=="" goto prepare
set "CLOSE=--close-on-exit"
set "CONFIG_ROOT=%%S37_CONFIGROOT%%"
set "PROGRAM=%%S37_CONFIGROOT%%\O37P.COM"
set "DIRECTORY=%%S37_CONFIGROOT%%"
if "%~3"=="--literal-startup" (
  set "PROGRAM=O:\winnt\tests\O37P.COM"
  set "DIRECTORY=%DEST%"
)
goto prepare
:xms
if "%~3"=="" exit /b 2
if not "%~4"=="" if not "%~4"=="--default-int15" if not "%~4"=="--default-low" if not "%~4"=="--reserved-high" exit /b 2
set "STEM=U35"
set "TITLE=S35 XMS profile"
set "PROGRAM=%~3\U35.COM"
set "CONFIG_ROOT=%~3"
set "MEMORY=--w386-min-ems 1024 --w386-max-xms 4096"
set "HIMEM=device=%%SystemRoot%%\system32\himem.sys"
if "%~4"=="" set "HIMEM=%HIMEM% /INT15=128"
if "%~4"=="--reserved-high" set "HIMEM=%HIMEM% /INT15=128"
:prepare
set "CANDIDATE=%DEST%\%STEM%.PIF.stage"
if exist "%CANDIDATE%" exit /b 2
copy /b "%TEMPLATE%" "%CANDIDATE%" >nul || exit /b 1
"%PIF%" update "%CANDIDATE%" --title "%TITLE%" --program "%PROGRAM%" --directory "%DIRECTORY%" --config "%CONFIG_ROOT%\%STEM%.NT" --autoexec "%CONFIG_ROOT%\%STEM%AUTO.NT" %MEMORY% %CLOSE%
if errorlevel 1 goto failed
call :config "%~4" >"%DEST%\%STEM%.NT"
if errorlevel 1 goto failed
>"%DEST%\%STEM%AUTO.NT" echo @echo off
if errorlevel 1 goto failed
if "%STEM%"=="U35" >>"%DEST%\%STEM%AUTO.NT" echo echo S35_XMS_PROFILE_LOADED
if errorlevel 1 goto failed
move /y "%CANDIDATE%" "%DEST%\%STEM%.PIF" >nul || goto failed
exit /b 0
:config
if "%STEM%"=="P30" (
  echo files=20
  exit /b %errorlevel%
)
echo EMM=RAM
if "%STEM%"=="E30" (
  echo dos=high, umb
  echo device=%%SystemRoot%%\system32\himem.sys
  exit /b %errorlevel%
)
if "%~1"=="--default-int15" echo dos=high
if "%~1"=="--reserved-high" echo dos=high
echo %HIMEM%
echo device=%%SystemRoot%%\tests\X35UMB.SYS
echo files=20
exit /b %errorlevel%
:failed
if exist "%CANDIDATE%" del /q "%CANDIDATE%"
exit /b 1
