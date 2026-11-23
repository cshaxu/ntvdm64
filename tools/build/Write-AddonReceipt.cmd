@echo off
setlocal EnableExtensions DisableDelayedExpansion
if "%~4"=="" exit /b 2
set "HASH=%~1"
set "IMAGE=%~2"
set "RECEIPT=%~3"
set "INPUTS=%~4"
set "HASH=%HASH:/=\%"
set "IMAGE=%IMAGE:/=\%"
set "RECEIPT=%RECEIPT:/=\%"
set "INPUTS=%INPUTS:/=\%"
"%HASH%" "%IMAGE%" >"%RECEIPT%.sha256.new"
if errorlevel 1 goto failed
copy /b /y "%INPUTS%" "%RECEIPT%.inputs.json.new" >nul
if errorlevel 1 goto failed
move /y "%RECEIPT%.sha256.new" "%RECEIPT%.sha256" >nul
if errorlevel 1 goto failed
move /y "%RECEIPT%.inputs.json.new" "%RECEIPT%.inputs.json" >nul
exit /b %errorlevel%
:failed
if exist "%RECEIPT%.sha256.new" del /q "%RECEIPT%.sha256.new"
if exist "%RECEIPT%.inputs.json.new" del /q "%RECEIPT%.inputs.json.new"
exit /b 1
