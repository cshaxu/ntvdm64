@echo off
setlocal
"%~1" /nologo /EP /TC /DFE_SB "%~3\msgdef.lst" > "%~4\msgdef.lst"
if errorlevel 1 exit /b 1
"%~2" "%~4\msgdef.lst" "%~3\msgdef.tpl" > "%~4\msgdef.h"
exit /b %errorlevel%
