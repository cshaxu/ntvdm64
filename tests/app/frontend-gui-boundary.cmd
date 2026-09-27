@echo off
"%~1" "%~2" > "%~3"
set "GUI_RESULT=%errorlevel%"
type "%~3"
echo GUI-RETURN-%GUI_RESULT%
exit /b %GUI_RESULT%
