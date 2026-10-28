@echo off
setlocal EnableExtensions EnableDelayedExpansion
set "RESULT=1"
set "TARGET=%~1"
for %%I in ("%~dp0..\..\assets\release") do set "RELEASE=%%~fI"
if not defined TARGET set /p "TARGET=Target prepared Windows 3.1 installation-media copy: "
if not defined TARGET (set "RESULT=64"& goto done)
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
set "PATCH=%TARGET%\PATCH"
set "WORK=%PATCH%\W31STAGE"
if not exist "%RELEASE%\HASH.EXE" (set "RESULT=3"& goto done)
if not exist "%TARGET%\MOUSE.DR_.BAK" (echo Missing original mouse backup.& set "RESULT=2"& goto done)
if exist "%WORK%\" (echo Preserve existing staging directory: %WORK%& set "RESULT=5"& goto done)
for %%F in (KRNL386 WIN386) do if not exist "%TARGET%\%%F.EX_.BAK" (echo Missing original backup: %%F& set "RESULT=2"& goto done)
if exist "%PATCH%\SETUP.CMD" fc /b "%PATCH%\SETUP.CMD" "%~dp0SETUP.CMD" >nul || (echo Preserve unrecognised PATCH\SETUP.CMD& set "RESULT=5"& goto done)
if exist "%PATCH%\PIF.EXE" fc /b "%PATCH%\PIF.EXE" "%RELEASE%\PIF.EXE" >nul || (echo Preserve unrecognised PATCH\PIF.EXE& set "RESULT=5"& goto done)
if exist "%PATCH%\MOUSE31.DRV" fc /b "%PATCH%\MOUSE31.DRV" "%RELEASE%\MOUSE31.DRV" >nul || (set "RESULT=5"& goto done)
mkdir "%WORK%" || (set "RESULT=4"& goto done)
expand.exe "%TARGET%\MOUSE.DR_" "%WORK%\MOUSE-CURRENT.DRV" >nul || (set "RESULT=6"& goto cleanup)
expand.exe "%TARGET%\MOUSE.DR_.BAK" "%WORK%\MOUSE-RETAIL.DRV" >nul || (set "RESULT=6"& goto cleanup)
fc /b "%WORK%\MOUSE-CURRENT.DRV" "%RELEASE%\MOUSE31.DRV" >nul || (echo Preserve unrecognised MOUSE.DR_& set "RESULT=5"& goto cleanup)
set "MH="
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%WORK%\MOUSE-RETAIL.DRV"') do set "MH=%%H"
if /i not "!MH!"=="B4F0B9DE8043071FA2B82B16373BF5188BBE8162C3EF24C8C3D1B01F86B96B07" (echo Invalid original mouse backup.& set "RESULT=5"& goto cleanup)
copy /y "%TARGET%\MOUSE.DR_" "%WORK%\MOUSE-CURRENT.DR_" >nul || (set "RESULT=6"& goto cleanup)
for %%F in (KRNL386 WIN386) do (
  expand.exe "%TARGET%\%%F.EX_" "%WORK%\%%F-CURRENT.EXE" >nul || (set "RESULT=6"& goto cleanup)
  expand.exe "%TARGET%\%%F.EX_.BAK" "%WORK%\%%F-RETAIL.EXE" >nul || (set "RESULT=6"& goto cleanup)
  copy /y "%TARGET%\%%F.EX_" "%WORK%\%%F-CURRENT.EX_" >nul || (set "RESULT=6"& goto cleanup)
)
set "KCUR="& set "WCUR="& set "KOLD="& set "WOLD="
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%WORK%\KRNL386-CURRENT.EXE"') do set "KCUR=%%H"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%WORK%\WIN386-CURRENT.EXE"') do set "WCUR=%%H"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%WORK%\KRNL386-RETAIL.EXE"') do set "KOLD=%%H"
for /f "delims=" %%H in ('call "%RELEASE%\HASH.EXE" "%WORK%\WIN386-RETAIL.EXE"') do set "WOLD=%%H"
if /i not "!KCUR!"=="75E931AE000FB7C70CF686DA6F7D3FCB032FDC0E0A4E3187ED68179B85E2CD4A" (set "RESULT=5"& goto cleanup)
if /i not "!WCUR!"=="C67E667E25E91C65EFB5ACDDEC437DA8B586037DA7DE502228CA8FBD806CF3F8" (set "RESULT=5"& goto cleanup)
if /i not "!KOLD!"=="FBEAF672EE9E917318CE8FCD1D560DB805DBFADF4777039ABBAAFF1A9C75B980" (set "RESULT=5"& goto cleanup)
if /i not "!WOLD!"=="6006860AE1003114583D70A1C6447247E1280A99633202912092A5F06A0C22A5" (set "RESULT=5"& goto cleanup)
for %%F in (KRNL386 WIN386) do if exist "%PATCH%\%%F-RETAIL.EXE" fc /b "%PATCH%\%%F-RETAIL.EXE" "%WORK%\%%F-RETAIL.EXE" >nul || (set "RESULT=5"& goto cleanup)
for %%F in (KRNL386 WIN386) do if exist "%PATCH%\%%F-ADAPTED.EXE" fc /b "%PATCH%\%%F-ADAPTED.EXE" "%WORK%\%%F-CURRENT.EXE" >nul || (set "RESULT=5"& goto cleanup)
copy /y "%TARGET%\MOUSE.DR_.BAK" "%TARGET%\MOUSE.DR_" >nul || (set "RESULT=6"& goto rollback)
for %%F in (KRNL386 WIN386) do copy /y "%TARGET%\%%F.EX_.BAK" "%TARGET%\%%F.EX_" >nul || (set "RESULT=6"& goto rollback)
del /q "%TARGET%\MOUSE.DR_.BAK" || (set "RESULT=6"& goto cleanup)
for %%F in (KRNL386 WIN386) do if exist "%TARGET%\%%F.EX_.BAK" del /q "%TARGET%\%%F.EX_.BAK" || (echo Restored media, but could not remove %%F backup& set "RESULT=6"& goto cleanup)
for %%F in (KRNL386 WIN386) do for %%S in (-RETAIL.EXE -ADAPTED.EXE) do if exist "%PATCH%\%%F%%S" del /q "%PATCH%\%%F%%S" || (echo Restored media, but could not remove %%F%%S& set "RESULT=6"& goto cleanup)
for %%F in (SETUP.CMD PIF.EXE MOUSE31.DRV) do if exist "%PATCH%\%%F" del /q "%PATCH%\%%F" || (echo Restored media, but could not remove %%F& set "RESULT=6"& goto cleanup)
set "RESULT=0"
goto cleanup
:rollback
if exist "%WORK%\MOUSE-CURRENT.DR_" copy /y "%WORK%\MOUSE-CURRENT.DR_" "%TARGET%\MOUSE.DR_" >nul
for %%F in (KRNL386 WIN386) do if exist "%WORK%\%%F-CURRENT.EX_" copy /y "%WORK%\%%F-CURRENT.EX_" "%TARGET%\%%F.EX_" >nul
:cleanup
if exist "%WORK%\" rmdir /s /q "%WORK%"
if exist "%PATCH%\" rmdir "%PATCH%" 2>nul
:done
echo Patch recovery exit code: %RESULT%
if not defined NTVDM_SETUP_TEST_NO_PAUSE pause
exit /b %RESULT%
