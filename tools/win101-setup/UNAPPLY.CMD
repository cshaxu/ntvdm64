@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET="
set "RESULT=1"
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
echo Windows 1.01 patch recovery
echo.
set /p "TARGET=Target prepared Windows 1.01 installation-media directory: "
if not defined TARGET (
  echo No directory entered.
  set "RESULT=64"
  goto done
)
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
for %%F in (MOUSE.DRV MOUSE.DRV.BAK) do if not exist "%TARGET%\%%F" (
  echo Missing prepared media file: %%F
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
fc /b "%~dp0MOUSE.DRV" "%TARGET%\MOUSE.DRV" >nul
if errorlevel 1 (
  echo Refusing to overwrite an unrecognised current MOUSE.DRV.
  set "RESULT=5"
  goto done
)
set "PATCH=%TARGET%\PATCH"
if exist "%PATCH%\" (
  for %%F in (SETUP.CMD SETVER.EXE) do if exist "%PATCH%\%%F" (
    fc /b "%~dp0%%F" "%PATCH%\%%F" >nul
    if errorlevel 1 (
      echo Refusing to delete an unrecognised PATCH\%%F.
      set "RESULT=5"
      goto done
    )
  )
  for %%F in (PIF.EXE HASH.EXE) do if exist "%PATCH%\%%F" (
    fc /b "%RELEASE%\%%F" "%PATCH%\%%F" >nul
    if errorlevel 1 (
      echo Refusing to delete an unrecognised PATCH\%%F.
      set "RESULT=5"
      goto done
    )
  )
)
copy /y "%TARGET%\MOUSE.DRV.BAK" "%TARGET%\MOUSE.DRV" >nul || (
  echo Could not restore MOUSE.DRV from MOUSE.DRV.BAK.
  set "RESULT=5"
  goto done
)
del /q "%TARGET%\MOUSE.DRV.BAK" || (
  echo Restored MOUSE.DRV but could not remove MOUSE.DRV.BAK.
  set "RESULT=5"
  goto done
)
if exist "%PATCH%\" (
  rem Only the four helpers were authenticated above.  Do not remove a
  rem same-named configuration or directory that could belong to the owner.
  for %%F in (SETUP.CMD PIF.EXE HASH.EXE SETVER.EXE) do if exist "%PATCH%\%%F" del /q "%PATCH%\%%F"
  rmdir "%PATCH%" 2>nul
)
set "RESULT=0"

:done
echo.
echo Patch recovery exit code: %RESULT%
pause
exit /b %RESULT%
