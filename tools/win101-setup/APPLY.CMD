@echo off
setlocal EnableExtensions EnableDelayedExpansion
set "TARGET="
set "RESULT=1"
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
echo Windows 1.01 patch preparation
echo.
set /p "TARGET=Target Windows 1.01 installation-media directory: "
if not defined TARGET (
  echo No directory entered.
  set "RESULT=64"
  goto done
)
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
for %%F in (SETUP.EXE SETUP.LBL KERNEL.EXE USER.EXE GDI.EXE DISK1 DISK2 DISK3 DISK4 DISK5 MOUSE.DRV) do if not exist "%TARGET%\%%F" (
  echo Missing original media file: %%F
  set "RESULT=2"
  goto done
)
for %%F in (SETUP.CMD SETVER.EXE MOUSE.DRV) do if not exist "%~dp0%%F" (
  echo Tool package is incomplete: %%F
  set "RESULT=3"
  goto done
)
for %%F in (PIF.EXE HASH.EXE) do if not exist "%RELEASE%\%%F" (
  echo Missing released add-on: %RELEASE%\%%F
  set "RESULT=3"
  goto done
)
set "NEW_HASH="
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%~dp0MOUSE.DRV"') do set "NEW_HASH=%%H"
if not defined NEW_HASH goto hash_failure
set "PATCH=%TARGET%\PATCH"
if not exist "%PATCH%\" mkdir "%PATCH%" || (
  echo Could not create "%PATCH%".
  set "RESULT=4"
  goto done
)
if exist "%PATCH%\MOUSE.DRV" (
  set "OLD_PATCH_HASH="
  for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%PATCH%\MOUSE.DRV"') do set "OLD_PATCH_HASH=%%H"
  if not defined OLD_PATCH_HASH goto hash_failure
  if /I not "!OLD_PATCH_HASH!"=="!NEW_HASH!" (
    echo Refusing to remove an unrecognised PATCH\MOUSE.DRV.
    set "RESULT=5"
    goto done
  )
  del /q "%PATCH%\MOUSE.DRV" || (
    echo Could not remove superseded PATCH\MOUSE.DRV.
    set "RESULT=5"
    goto done
  )
)
if exist "%TARGET%\MOUSE.DRV.BAK" (
  set "CURRENT_HASH="
  for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%TARGET%\MOUSE.DRV"') do set "CURRENT_HASH=%%H"
  if not defined CURRENT_HASH goto hash_failure
  if /I not "!CURRENT_HASH!"=="!NEW_HASH!" (
    echo Existing MOUSE.DRV.BAK does not match an already-applied replacement.
    set "RESULT=5"
    goto done
  )
) else (
  move /y "%TARGET%\MOUSE.DRV" "%TARGET%\MOUSE.DRV.BAK" >nul || (
    echo Could not preserve original MOUSE.DRV as MOUSE.DRV.BAK.
    set "RESULT=5"
    goto done
  )
  copy /y "%~dp0MOUSE.DRV" "%TARGET%\MOUSE.DRV" >nul || (
    echo Could not install replacement MOUSE.DRV.
    move /y "%TARGET%\MOUSE.DRV.BAK" "%TARGET%\MOUSE.DRV" >nul 2>nul
    set "RESULT=5"
    goto done
  )
)
for %%F in (SETUP.CMD SETVER.EXE) do copy /y "%~dp0%%F" "%PATCH%\%%F" >nul || (
  echo Could not copy %%F.
  set "RESULT=5"
  goto done
)
for %%F in (PIF.EXE HASH.EXE) do copy /y "%RELEASE%\%%F" "%PATCH%\%%F" >nul || (
  echo Could not copy released %%F.
  set "RESULT=5"
  goto done
)
for %%F in (README.TXT CONFIG.NT AUTOEXEC.NT SETUP.PIF setup-result.txt configure-launch.ps1 run-setup.ps1 win.cmd.template WIN31-TEMPLATE.PIF addon-files.json) do if exist "%PATCH%\%%F" del /q "%PATCH%\%%F"
if exist "%PATCH%\TEMP\" rmdir /s /q "%PATCH%\TEMP"
set "RESULT=0"

:done
echo.
echo Patch preparation exit code: %RESULT%
pause
exit /b %RESULT%

:hash_failure
echo Could not calculate the mouse-driver identity.
set "RESULT=5"
goto done
