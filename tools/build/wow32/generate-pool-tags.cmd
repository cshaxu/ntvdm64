@echo off
setlocal
pushd "%~3"
if errorlevel 1 exit /b 1
"%~1" "%~2\ptag.lst" "%~2\ptag.tpl" > ptag.tmp
if errorlevel 1 goto failed
call "%~2\ptag.bat" ptag.tmp ptag.h
if errorlevel 1 goto failed
del ptag.tmp
popd
exit /b 0
:failed
if exist ptag.tmp del ptag.tmp
popd
exit /b 1
