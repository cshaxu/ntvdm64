@echo off
setlocal EnableExtensions DisableDelayedExpansion
set "TARGET=%~1"
if not defined TARGET set /p "TARGET=Target repaired Windows 95 directory: "
if not defined TARGET exit /b 64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Undo-Win95Path.ps1" -WindowsRoot "%TARGET%"
exit /b %errorlevel%
