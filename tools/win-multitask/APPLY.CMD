@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET=%~1"
if not defined TARGET set /p "TARGET=Installed Windows directory: "
if not defined TARGET exit /b 64
for %%I in ("%TARGET%") do set "TARGET=%%~sI"
if not exist "%TARGET%\WIN.COM" (echo Missing WIN.COM.& exit /b 2)
if not exist "%TARGET%\SYSTEM.INI" (echo Missing SYSTEM.INI.& exit /b 2)
set "IS31=" & set "IS95="
if exist "%TARGET%\SYSTEM\WIN386.EXE" set "IS31=1"
if exist "%TARGET%\SYSTEM\VMM32.VXD" set "IS95=1"
if not defined IS31 if not defined IS95 (echo Unsupported or incomplete Windows installation.& exit /b 2)
if defined IS31 if defined IS95 (echo Ambiguous Windows installation.& exit /b 2)
if not "%TARGET: =%"=="%TARGET%" (echo An ASCII short path without spaces is required.& exit /b 5)
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
for %%F in (INSIGNIA.386 NTVDM64.386 HASH.EXE) do if not exist "%RELEASE%\%%F" (echo Missing released component %%F.& exit /b 3)
set "PATCH=%TARGET%\PATCH"
set "INI=%TARGET%\SYSTEM.INI"
for %%F in (INSIGNIA.386 NTVDM64.386 MULTITASK.INI.BAK MULTITASK.INI.NEW MULTITASK.HASH) do if exist "%PATCH%\%%F" (echo Existing file blocks installation: %%F.& exit /b 5)
findstr /l /c:"!" "%INI%" >nul && (echo SYSTEM.INI contains unsupported exclamation marks.& exit /b 5)
findstr /i /l /c:"INSIGNIA.386" /c:"NTVDM64.386" /c:"NTVDM64.VXD" "%INI%" >nul && (echo Existing multitask entries require inspection.& exit /b 5)
set "NEED_VMD="
if defined IS31 (
  set "NEED_VMD=1"
  setlocal EnableDelayedExpansion
  set "IN_386="
  set "FOUND_VMD="
  for /f "tokens=1,* delims=:" %%A in ('findstr /n "^" "%INI%"') do (
    set "LINE=%%B"
    if "!LINE:~0,1!"=="[" set "IN_386="
    if /i "!LINE:~0,8!"=="[386Enh]" set "IN_386=1"
    if defined IN_386 for /f "tokens=1,* delims==" %%K in ("!LINE!") do (
      set "KEY=%%K"
      set "VALUE=%%L"
      for /f "tokens=1 delims=;" %%V in ("!VALUE!") do set "VALUE=%%V"
      set "KEY=!KEY: =!"
      set "KEY=!KEY:	=!"
      set "VALUE=!VALUE: =!"
      set "VALUE=!VALUE:	=!"
      if /i "!KEY!"=="device" if /i "!VALUE!"=="*vmd" set "FOUND_VMD=1"
    )
  )
  if defined FOUND_VMD (endlocal & set "NEED_VMD=") else endlocal
)
if not exist "%PATCH%\" mkdir "%PATCH%" || exit /b 4
set "FOUND=0"
setlocal EnableDelayedExpansion
set "IN_386="
>"%PATCH%\MULTITASK.INI.NEW" (
  for /f "delims=" %%L in ('findstr /n "^" "%INI%"') do (
    set "LINE=%%L"
    set "LINE=!LINE:*:=!"
    if "!LINE:~0,1!"=="[" set "IN_386="
    if /i "!LINE:~0,8!"=="[386Enh]" set "IN_386=1"
    set "SKIP="
    if defined IS31 if defined IN_386 for /f "tokens=1,* delims==" %%K in ("!LINE!") do (
      set "KEY=%%K"
      set "VALUE=%%L"
      for /f "tokens=1 delims=;" %%V in ("!VALUE!") do set "VALUE=%%V"
      set "KEY=!KEY: =!"
      set "KEY=!KEY:	=!"
      set "VALUE=!VALUE: =!"
      set "VALUE=!VALUE:	=!"
      if /i "!KEY!"=="mouse" if /i "!VALUE!"=="*vmd" set "SKIP=1"
    )
    if not defined SKIP echo(!LINE!
    if /i "!LINE:~0,8!"=="[386Enh]" (
      set /a FOUND+=1
      if defined NEED_VMD echo(device=*vmd
      echo(device=!TARGET!\PATCH\INSIGNIA.386
      echo(device=!TARGET!\PATCH\NTVDM64.386
    )
  )
)
if not "!FOUND!"=="1" (del /q "%PATCH%\MULTITASK.INI.NEW" & echo Exactly one [386Enh] section is required.& exit /b 5)
endlocal
copy /b "%INI%" "%PATCH%\MULTITASK.INI.BAK" >nul || goto fail
copy /b "%RELEASE%\INSIGNIA.386" "%PATCH%\INSIGNIA.386" >nul || goto fail
copy /b "%RELEASE%\NTVDM64.386" "%PATCH%\NTVDM64.386" >nul || goto fail
"%RELEASE%\HASH.EXE" "%PATCH%\MULTITASK.INI.BAK" >"%PATCH%\MULTITASK.HASH" || goto fail
copy /b /y "%PATCH%\MULTITASK.INI.NEW" "%INI%" >nul || goto fail
echo Multitask drivers installed in %TARGET%\PATCH. Restart Windows.
exit /b 0
:fail
if exist "%PATCH%\MULTITASK.INI.BAK" (
  copy /b /y "%PATCH%\MULTITASK.INI.BAK" "%INI%" >nul || goto restore_failed
  fc /b "%PATCH%\MULTITASK.INI.BAK" "%INI%" >nul || goto restore_failed
)
for %%F in (INSIGNIA.386 NTVDM64.386 MULTITASK.INI.BAK MULTITASK.INI.NEW MULTITASK.HASH) do if exist "%PATCH%\%%F" del /q "%PATCH%\%%F"
echo Installation failed.
exit /b 5
:restore_failed
echo Installation failed and the original configuration could not be restored.
echo Backup and installation files were retained in %PATCH%.
exit /b 5
