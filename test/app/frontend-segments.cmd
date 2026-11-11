@echo off
"%~1" "%~3" --identity BEFORE
if not errorlevel 37 exit /b 80
if errorlevel 38 exit /b 81
"%~1" --wait "%~2" "%~1" --wait "%~2" "%~1" --wait "%~2" "%~1" "%~3" --identity INNER > "%~4"
set "GUI_RESULT=%errorlevel%"
type "%~4"
echo GUI-SEGMENT-RETURN-%GUI_RESULT%
if not "%GUI_RESULT%"=="37" exit /b 82
"%~1" "%~3" --identity AFTER
exit /b %errorlevel%
