@echo off
setlocal EnableExtensions EnableDelayedExpansion
set "RESULT=1"
set "TARGET=%~1"
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
if not defined TARGET set /p "TARGET=Target Windows 3.1 installation-media copy: "
if not defined TARGET (set "RESULT=64"& goto done)
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
for %%F in (SETUP.EXE DOSX.EX_ KRNL386.EX_ WIN386.EX_ MOUSE.DR_) do if not exist "%TARGET%\%%F" (echo Missing media file: %%F& set "RESULT=2"& goto done)
for %%F in (SETUP.CMD) do if not exist "%~dp0%%F" (echo Missing tool file: %%F& set "RESULT=3"& goto done)
for %%F in (HASH.EXE MOUSE31.DRV PIF.EXE SZDD.EXE W31PATCH.EXE) do if not exist "%RELEASE%\%%F" (echo Missing release file: %%F& set "RESULT=3"& goto done)
set "PATCH=%TARGET%\PATCH"
set "WORK=%PATCH%\W31STAGE"
if exist "%WORK%\" (echo Preserve existing staging directory: %WORK%& set "RESULT=5"& goto done)
if not exist "%PATCH%\" mkdir "%PATCH%" || (set "RESULT=4"& goto done)
mkdir "%WORK%" || (set "RESULT=4"& goto done)
for %%F in (SETUP.CMD) do if exist "%PATCH%\%%F" (
  fc /b "%PATCH%\%%F" "%~dp0%%F" >nul || (echo Preserve unrecognised PATCH\%%F& set "RESULT=5"& goto cleanup)
)
if exist "%PATCH%\MOUSE31.DRV" fc /b "%PATCH%\MOUSE31.DRV" "%RELEASE%\MOUSE31.DRV" >nul || (echo Preserve unrecognised PATCH\MOUSE31.DRV& set "RESULT=5"& goto cleanup)
if exist "%PATCH%\PIF.EXE" fc /b "%PATCH%\PIF.EXE" "%RELEASE%\PIF.EXE" >nul || (echo Preserve unrecognised PATCH\PIF.EXE& set "RESULT=5"& goto cleanup)
copy /y "%TARGET%\MOUSE.DR_" "%WORK%\MOUSE-CURRENT.DR_" >nul || (set "RESULT=6"& goto cleanup)
if exist "%TARGET%\MOUSE.DR_.BAK" (
  expand.exe "%TARGET%\MOUSE.DR_.BAK" "%WORK%\MOUSE-RETAIL.DRV" >nul || (set "RESULT=6"& goto cleanup)
  expand.exe "%TARGET%\MOUSE.DR_" "%WORK%\MOUSE-CURRENT.DRV" >nul || (set "RESULT=6"& goto cleanup)
  fc /b "%WORK%\MOUSE-CURRENT.DRV" "%RELEASE%\MOUSE31.DRV" >nul || fc /b "%TARGET%\MOUSE.DR_" "%TARGET%\MOUSE.DR_.BAK" >nul || (echo Preserve unrecognised MOUSE.DR_& set "RESULT=5"& goto cleanup)
) else (
  expand.exe "%TARGET%\MOUSE.DR_" "%WORK%\MOUSE-RETAIL.DRV" >nul || (set "RESULT=6"& goto cleanup)
)
set "MH="
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%WORK%\MOUSE-RETAIL.DRV"') do set "MH=%%H"
if /i not "!MH!"=="B4F0B9DE8043071FA2B82B16373BF5188BBE8162C3EF24C8C3D1B01F86B96B07" (echo Missing or unrecognised original mouse driver.& set "RESULT=6"& goto cleanup)
"%RELEASE%\SZDD.EXE" "%RELEASE%\MOUSE31.DRV" "%WORK%\MOUSE.DR_" || (set "RESULT=6"& goto cleanup)
expand.exe "%WORK%\MOUSE.DR_" "%WORK%\MOUSE-CHECK.DRV" >nul || (set "RESULT=6"& goto cleanup)
fc /b "%WORK%\MOUSE-CHECK.DRV" "%RELEASE%\MOUSE31.DRV" >nul || (set "RESULT=6"& goto cleanup)
for %%F in (KRNL386 WIN386) do (
  expand.exe "%TARGET%\%%F.EX_" "%WORK%\%%F.EXE" >nul || (set "RESULT=6"& goto cleanup)
  if exist "%TARGET%\%%F.EX_.BAK" (
    if exist "%PATCH%\%%F.EX_.BAK" fc /b "%TARGET%\%%F.EX_.BAK" "%PATCH%\%%F.EX_.BAK" >nul || (echo Conflicting %%F backups& set "RESULT=5"& goto cleanup)
    expand.exe "%TARGET%\%%F.EX_.BAK" "%WORK%\%%F-RETAIL.EXE" >nul || (set "RESULT=6"& goto cleanup)
  ) else if exist "%PATCH%\%%F.EX_.BAK" (
    expand.exe "%PATCH%\%%F.EX_.BAK" "%WORK%\%%F-RETAIL.EXE" >nul || (set "RESULT=6"& goto cleanup)
  ) else (
    copy /y "%WORK%\%%F.EXE" "%WORK%\%%F-RETAIL.EXE" >nul || (set "RESULT=6"& goto cleanup)
  )
)
set "KH="& set "WH="
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%WORK%\KRNL386-RETAIL.EXE"') do set "KH=%%H"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%WORK%\WIN386-RETAIL.EXE"') do set "WH=%%H"
if /i not "!KH!"=="FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980" (echo Missing or unrecognised original KRNL386 backup.& set "RESULT=6"& goto cleanup)
if /i not "!WH!"=="6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5" (echo Missing or unrecognised original WIN386 backup.& set "RESULT=6"& goto cleanup)
"%RELEASE%\W31PATCH.EXE" "%WORK%\KRNL386.EXE" "%WORK%\WIN386.EXE" || (set "RESULT=6"& goto cleanup)
for %%F in (KRNL386 WIN386) do (
  "%RELEASE%\SZDD.EXE" "%WORK%\%%F.EXE" "%WORK%\%%F.EX_" || (set "RESULT=6"& goto cleanup)
  expand.exe "%WORK%\%%F.EX_" "%WORK%\%%F-CHECK.EXE" >nul || (set "RESULT=6"& goto cleanup)
  fc /b "%WORK%\%%F.EXE" "%WORK%\%%F-CHECK.EXE" >nul || (set "RESULT=6"& goto cleanup)
  copy /y "%TARGET%\%%F.EX_" "%WORK%\%%F-CURRENT.EX_" >nul || (set "RESULT=6"& goto cleanup)
)
for %%F in (KRNL386 WIN386) do (
  if exist "%PATCH%\%%F-RETAIL.EXE" fc /b "%PATCH%\%%F-RETAIL.EXE" "%WORK%\%%F-RETAIL.EXE" >nul || (echo Preserve unrecognised %%F-RETAIL.EXE& set "RESULT=5"& goto cleanup)
  if exist "%PATCH%\%%F-ADAPTED.EXE" fc /b "%PATCH%\%%F-ADAPTED.EXE" "%WORK%\%%F.EXE" >nul || (echo Preserve unrecognised %%F-ADAPTED.EXE& set "RESULT=5"& goto cleanup)
)
for %%F in (KRNL386 WIN386) do if not exist "%TARGET%\%%F.EX_.BAK" (
  if exist "%PATCH%\%%F.EX_.BAK" (
    copy /y "%PATCH%\%%F.EX_.BAK" "%TARGET%\%%F.EX_.BAK" >nul || (set "RESULT=6"& goto cleanup)
  ) else (
    copy /y "%WORK%\%%F-CURRENT.EX_" "%TARGET%\%%F.EX_.BAK" >nul || (set "RESULT=6"& goto cleanup)
  )
)
if not exist "%TARGET%\MOUSE.DR_.BAK" copy /y "%WORK%\MOUSE-CURRENT.DR_" "%TARGET%\MOUSE.DR_.BAK" >nul || (set "RESULT=6"& goto cleanup)
copy /y "%WORK%\MOUSE.DR_" "%TARGET%\MOUSE.DR_" >nul || (set "RESULT=6"& goto rollback)
for %%F in (KRNL386 WIN386) do (
  copy /y "%WORK%\%%F.EX_" "%TARGET%\%%F.EX_" >nul || (set "RESULT=6"& goto rollback)
)
copy /y "%~dp0SETUP.CMD" "%PATCH%\SETUP.CMD" >nul || (set "RESULT=6"& goto rollback)
copy /y "%RELEASE%\PIF.EXE" "%PATCH%\PIF.EXE" >nul || (set "RESULT=6"& goto rollback)
for %%F in (KRNL386 WIN386) do if exist "%PATCH%\%%F.EX_.BAK" del /q "%PATCH%\%%F.EX_.BAK" || (set "RESULT=6"& goto cleanup)
set "RESULT=0"
echo Prepared %PATCH%\SETUP.CMD
goto cleanup
:rollback
if exist "%WORK%\MOUSE-CURRENT.DR_" copy /y "%WORK%\MOUSE-CURRENT.DR_" "%TARGET%\MOUSE.DR_" >nul
for %%F in (KRNL386 WIN386) do if exist "%WORK%\%%F-CURRENT.EX_" copy /y "%WORK%\%%F-CURRENT.EX_" "%TARGET%\%%F.EX_" >nul
:cleanup
if exist "%WORK%\" rmdir /s /q "%WORK%"
:done
echo Patch preparation exit code: %RESULT%
if not defined NTVDM_SETUP_TEST_NO_PAUSE pause
exit /b %RESULT%
