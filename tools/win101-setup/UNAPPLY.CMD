@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET="
set /p "TARGET=Target prepared Windows 1.01 installation-media directory: "
if not defined TARGET exit /b 64
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
if not exist "%TARGET%\MOUSE.DRV.BAK" (echo Missing MOUSE.DRV.BAK.& exit /b 2)
copy /y "%TARGET%\MOUSE.DRV.BAK" "%TARGET%\MOUSE.DRV" >nul || exit /b 5
del /q "%TARGET%\MOUSE.DRV.BAK" || exit /b 5
if exist "%TARGET%\PATCH\" rmdir /s /q "%TARGET%\PATCH" || exit /b 5
echo Windows 1.01 media restored.
exit /b 0
