@echo off
setlocal
if not "%~1"=="" goto explicit_arguments
call "%~dp0APPLY.CMD"
exit /b %errorlevel%

:explicit_arguments
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0apply-setup.ps1" %*
set "apply_result=%errorlevel%"
echo Patch preparation exit code: %apply_result%
pause
exit /b %apply_result%
