@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET=" & set "RESULT=1"
echo Windows 3.1 path repair recovery
echo.
set /p "TARGET=Target repaired Windows 3.1 directory: "
if not defined TARGET (set "RESULT=64"& goto done)
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
set "PATCH=%TARGET%\PATCH"
set "MANIFEST=%PATCH%\PATH-REPAIR.MANIFEST"
if not exist "%MANIFEST%" (echo Missing path-repair recovery manifest.& set "RESULT=2"& goto done)
set "RESTORE_FAILED="
for /f "usebackq delims=" %%A in ("%MANIFEST%") do call :restore "%%A"
if defined RESTORE_FAILED goto mutation_failure
if exist "%MANIFEST%" echo Remove: PATH-REPAIR.MANIFEST =^> %PATCH%
if exist "%PATCH%\PATH-REPAIR.TXT" echo Remove: PATH-REPAIR.TXT =^> %PATCH%
del /q "%MANIFEST%" "%PATCH%\PATH-REPAIR.TXT" 2>nul
rmdir "%PATCH%" 2>nul
set "RESULT=0"
goto done

:restore
if not exist "%TARGET%\%~1.BAK" (set "RESTORE_FAILED=1"& exit /b 1)
for %%I in ("%TARGET%\%~1") do echo Restore: %%~nxI =^> %%~dpI
move /y "%TARGET%\%~1.BAK" "%TARGET%\%~1" >nul || (set "RESTORE_FAILED=1"& exit /b 1)
exit /b 0

:mutation_failure
echo Recovery stopped; remaining files were preserved for inspection.
set "RESULT=5"

:done
echo.
echo Path recovery exit code: %RESULT%
pause
exit /b %RESULT%
