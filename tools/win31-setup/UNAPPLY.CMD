@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET="
set /p "TARGET=Target prepared Windows 3.1 installation-media copy: "
if not defined TARGET exit /b 64
for %%I in ("%TARGET%") do set "TARGET=%%~fI"
for %%F in (MOUSE.DR_ KRNL386.EX_ WIN386.EX_) do if not exist "%TARGET%\%%F.BAK" (echo Missing backup: %%F.BAK& exit /b 2)
for %%F in (MOUSE.DR_ KRNL386.EX_ WIN386.EX_) do copy /y "%TARGET%\%%F.BAK" "%TARGET%\%%F" >nul || exit /b 5
for %%F in (MOUSE.DR_ KRNL386.EX_ WIN386.EX_) do del /q "%TARGET%\%%F.BAK" || exit /b 5
if exist "%TARGET%\PATCH\" rmdir /s /q "%TARGET%\PATCH" || exit /b 5
echo Windows 3.1 media restored.
exit /b 0
