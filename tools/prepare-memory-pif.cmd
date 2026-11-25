@echo off
rem Generate the EMS test profile using the existing PIF editor.
setlocal EnableExtensions DisableDelayedExpansion
if "%~1"=="" exit /b 2
if not "%~3"=="" exit /b 2
if not "%~2"=="" if not "%~2"=="--close-on-exit" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "DEST=%~1"
if not exist "%DEST%\" exit /b 2
set "PIF=%ROOT%\assets\release\PIF.EXE"
if defined NTVDM_TEST_PIF_EXE set "PIF=%NTVDM_TEST_PIF_EXE%"
if not exist "%PIF%" exit /b 2
set "TEMPLATE=%ROOT%\test\observation\fixtures\memory-profile.pif"
set "CANDIDATE=%DEST%\E30.PIF.stage"
if exist "%CANDIDATE%" exit /b 2
copy /b "%TEMPLATE%" "%CANDIDATE%" >nul || exit /b 1
"%PIF%" update "%CANDIDATE%" --title "S30 EMS capability" --program E30.COM --config "%DEST%\E30.NT" --autoexec "%DEST%\E30AUTO.NT" --w386-min-ems 1024 %2
if errorlevel 1 goto failed
(
 echo EMM=RAM
 echo dos=high, umb
 echo device=%%SystemRoot%%\system32\himem.sys
) >"%DEST%\E30.NT"
if errorlevel 1 goto failed
>"%DEST%\E30AUTO.NT" echo @echo off
if errorlevel 1 goto failed
move /y "%CANDIDATE%" "%DEST%\E30.PIF" >nul || goto failed
exit /b 0
:failed
if exist "%CANDIDATE%" del /q "%CANDIDATE%"
exit /b 1
