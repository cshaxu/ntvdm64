@echo off
setlocal EnableExtensions DisableDelayedExpansion
if not "%~3"=="" exit /b 2
if "%~2"=="" exit /b 2
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "STAGE=%~f1"
set "OUTPUT=%~f2"
for %%I in ("%STAGE%\..") do if /i not "%%~fI"=="O:\tmp" exit /b 2
if not exist "%STAGE%\.ntvdm64-test-stage.json" exit /b 2
if not exist "%OUTPUT%\" exit /b 2
set "PARENT=%OUTPUT%"
:check_parent
set "PREVIOUS=%PARENT%"
for %%I in ("%PARENT%\..") do set "PARENT=%%~fI"
if /i "%PARENT%"=="%ROOT%\build" goto prepare
if /i "%PARENT%"=="%PREVIOUS%" exit /b 2
goto check_parent
:prepare
set "BINARY=%STAGE%"
if exist "%STAGE%\system32\run16.exe" set "BINARY=%STAGE%\system32"
set "CACHE=%ROOT%\build\.ninja\msvc-x86-release\worker"
if not exist "%BINARY%\MEM.EXE" exit /b 2
set "PIF=%ROOT%\assets\release\PIF.EXE"
if not exist "%PIF%" exit /b 2
if not exist "%CACHE%\s45-cmdkeyb-shadow-fixture.exe" exit /b 2
if exist "%BINARY%\KTEST" exit /b 2
if exist "%OUTPUT%\system32\s45-cmdkeyb-shadow-fixture.exe" exit /b 2
if not exist "%OUTPUT%\system32\" mkdir "%OUTPUT%\system32"
if errorlevel 1 exit /b 1
copy /b "%CACHE%\s45-cmdkeyb-shadow-fixture.exe" "%OUTPUT%\system32\s45-cmdkeyb-shadow-fixture.exe" >nul
if errorlevel 1 exit /b 1
mkdir "%BINARY%\KTEST"
if errorlevel 1 exit /b 1
set "PROFILE=%BINARY%\KTEST\S45KEY.PIF"
copy /b "%ROOT%\test\observation\fixtures\keyboard-profile.pif" "%PROFILE%.stage" >nul || exit /b 1
"%PIF%" update "%PROFILE%.stage" --program "%BINARY%\MEM.EXE" --config "%BINARY%\config.nt" --autoexec "%BINARY%\autoexec.nt"
if errorlevel 1 exit /b %errorlevel%
move /y "%PROFILE%.stage" "%PROFILE%" >nul || exit /b 1
copy /b /y "%ROOT%\test\mvdm-host\fixtures\S45-KEYBOARD-NTVDM.REG" "%BINARY%\NTVDM.REG" >nul
exit /b %errorlevel%
